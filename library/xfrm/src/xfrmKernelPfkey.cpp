#include <unistd.h>
#include <linux/pfkeyv2.h>
#include <netinet/in.h>
#include "mxLog.h"
#include "xfrmKernelPfkey.h"

xfrmKernelPfkey::xfrmKernelPfkey()
{
	mSocket = 0;
	mSocketEvents = 0;
	mSequence = 0;
	mConfigSpiMin = KERNEL_SPI_MIN;
	mConfigSpiMax = KERNEL_SPI_MAX;

}

xfrmKernelPfkey::~xfrmKernelPfkey()
{
	if (mSocket != 0) {
		close(mSocket);
		mSocket = 0;
	}
	if (mSocketEvents != 0) {
		close(mSocketEvents);
		mSocketEvents = 0;
	}

}

u32_t xfrmKernelPfkey::createSocket(u32_t registerEvents)
{
	/* create a PF_KEY socket to communicate with the kernel */
	mSocket = socket(PF_KEY, SOCK_RAW, PF_KEY_V2);
	if (mSocket <= 0) {
		mxLogFmt(LOG_DEBUG,"unable to create pfkey socket: %s (%d)\n",strerror(errno), errno);
		return STATUS_FAILED;
	}

	if (registerEvents) {
		/* create a PF_KEY socket for ACQUIRE & EXPIRE */
		mSocketEvents = socket(PF_KEY, SOCK_RAW, PF_KEY_V2);
		u32_t rcv_buffer = MAX_RCV_BUFFER;
		if (rcv_buffer > 0) {
			setsockopt(mSocketEvents, SOL_SOCKET, SO_RCVBUF, &rcv_buffer, sizeof(rcv_buffer));
		}
	}
	/* register the event socket */
	if (RegisterPfkeySocket(SADB_SATYPE_ESP) != STATUS_SUCCESS ||
		RegisterPfkeySocket(SADB_SATYPE_AH) != STATUS_SUCCESS)
	{
		mxLogFmt(LOG_DEBUG,"unable to register PF_KEY event socket\n");
		return STATUS_FAILED;
	}
	return STATUS_SUCCESS;
}

/**
 * Get an SPI for a specific protocol from the kernel.
 */
u32_t xfrmKernelPfkey::getSpi(struct sockaddr *pSrc, struct sockaddr *pDst, u8_t protocol, u32_t& spi)
{
	unsigned char request[PFKEY_BUFFER_SIZE];
	struct sadb_msg *msg, *out;
	struct sadb_spirange *range;
	//pfkey_msg_t response;
	u32_t received_spi = 0;
	size_t len;

	memset(&request, 0, sizeof(request));

	msg = (struct sadb_msg*)request;
	msg->sadb_msg_version = PF_KEY_V2;
	msg->sadb_msg_type = SADB_GETSPI;
	msg->sadb_msg_satype = convertProtoToSaType(protocol);
	msg->sadb_msg_len = PFKEY_LEN(sizeof(struct sadb_msg));
	msg->sadb_msg_seq = mSequence;
	msg->sadb_msg_pid = getpid();    /*unistd.h*/


	addAddrExt(msg, pSrc, SADB_EXT_ADDRESS_SRC, 0, 0);
	addAddrExt(msg, pDst, SADB_EXT_ADDRESS_DST, 0, 0);

	range = (struct sadb_spirange*)PFKEY_EXT_ADD_NEXT(msg);
	range->sadb_spirange_exttype = SADB_EXT_SPIRANGE;
	range->sadb_spirange_len = PFKEY_LEN(sizeof(struct sadb_spirange));
	range->sadb_spirange_min = mConfigSpiMin;
	range->sadb_spirange_max = mConfigSpiMax;
	PFKEY_EXT_ADD(msg, range);

	if (sendPfkeySocket(mSocket ,msg, &out, &len) == STATUS_SUCCESS) {
		if (out->sadb_msg_errno) {
			mxLogFmt(LOG_DEBUG,"allocating SPI failed: %s (%d)\n",strerror(out->sadb_msg_errno), out->sadb_msg_errno);
		} else {
			struct sadb_ext* ext = NULL;
			
			len -= sizeof(struct sadb_msg);
			ext = (struct sadb_ext*)(((char*)out) + sizeof(struct sadb_msg));
			
			while (PFKEY_EXT_OK(ext,len)) {
				if ((ext->sadb_ext_type > SADB_EXT_MAX) || (!ext->sadb_ext_type)) {
					mxLogFmt(LOG_DEBUG,"type of PF_KEY extension (%d) is invalid\n",ext->sadb_ext_type);
					break;
				}

				if (ext->sadb_ext_type == SADB_EXT_SA) {
					struct sadb_sa *sa = (struct sadb_sa *)ext;
					received_spi = sa->sadb_sa_spi;
					mxLogFmt(LOG_DEBUG,"allocating SPI %08x sucess\n",received_spi);
					break;
						
				}
				ext = PFKEY_EXT_NEXT_LEN(ext, len);
			}
		}
		free(out);
	}
	
	if (received_spi == 0) {
		return STATUS_FAILED;
	}
	spi = received_spi;
	
	return STATUS_SUCCESS;
}

/**
 * Register a socket for ACQUIRE/EXPIRE messages
 */
u32_t xfrmKernelPfkey::RegisterPfkeySocket(u8_t sa_type)
{
	u8_t request[PFKEY_BUFFER_SIZE];
	struct sadb_msg *msg, *out;
	size_t len;

	memset(&request, 0, sizeof(request));

	msg = (struct sadb_msg*)request;
	msg->sadb_msg_version = PF_KEY_V2;
	msg->sadb_msg_type = SADB_REGISTER;
	msg->sadb_msg_satype = sa_type;
	msg->sadb_msg_len = PFKEY_LEN(sizeof(struct sadb_msg));
	
	if (sendPfkeySocket(mSocketEvents, msg, &out, &len) != STATUS_SUCCESS)
	{
		mxLogFmt(LOG_DEBUG,"unable to register PF_KEY socket\n");
		return STATUS_FAILED;
	}
	else if (out->sadb_msg_errno)
	{
		mxLogFmt(LOG_DEBUG,"unable to register PF_KEY socket: %s (%d)\n", 
					   		strerror(out->sadb_msg_errno), out->sadb_msg_errno);
		free(out);
		return STATUS_FAILED;
	}
	free(out);
	
	return STATUS_SUCCESS;
}

/*
<linux/pfkeyv2.h>

struct sadb_msg {
	uint8_t		sadb_msg_version;
	uint8_t		sadb_msg_type;
	uint8_t		sadb_msg_errno;
	uint8_t		sadb_msg_satype;
	uint16_t	sadb_msg_len;
	uint16_t	sadb_msg_reserved;
	uint32_t	sadb_msg_seq;
	uint32_t	sadb_msg_pid;
} __attribute__((packed));

*/
/**
 * Send a message to a specific PF_KEY socket and handle the response.
 */
u32_t xfrmKernelPfkey::sendPfkeySocket(s32_t socket,struct sadb_msg *in, struct sadb_msg **out, size_t *out_len)
{
	unsigned char buf[PFKEY_BUFFER_SIZE];
	struct sadb_msg *msg;
	int in_len, len;

	memset(buf, 0, sizeof(buf));
	mMutex.lock();
	in_len = PFKEY_USER_LEN(in->sadb_msg_len);
	while (true)
	{
		len = send(socket, in, in_len, 0);

		if (len != in_len)
		{
			if (errno == EINTR)
			{
				/* interrupted, try again */
				continue;
			}
			mMutex.unlock();
			mxLogFmt(LOG_DEBUG,"error sending to PF_KEY socket: %s\n",strerror(errno));
			return STATUS_FAILED;
		}
		break;
	}

	while (true)
	{
		msg = (struct sadb_msg*)buf;

		len = recv(socket, buf, sizeof(buf), 0);

		if (len < 0)
		{
			if (errno == EINTR)
			{
				mxLogFmt(LOG_DEBUG,"got interrupted\n");
				/* interrupted, try again */
				continue;
			}

			mxLogFmt(LOG_DEBUG,"error reading from PF_KEY socket: %s\n",strerror(errno));
			mMutex.unlock();
			return STATUS_FAILED;
		}
		if (len < sizeof(struct sadb_msg) ||
			msg->sadb_msg_len < PFKEY_LEN(sizeof(struct sadb_msg)))
		{
			mxLogFmt(LOG_DEBUG,"received corrupted PF_KEY message\n");
			mMutex.unlock();
			return STATUS_FAILED;
		}
		if (msg->sadb_msg_len > len / PFKEY_ALIGNMENT)
		{
			mxLogFmt(LOG_DEBUG,"buffer was too small to receive the complete PF_KEY message\n");
			mMutex.unlock();
			return STATUS_FAILED;
		}
		if (msg->sadb_msg_pid != in->sadb_msg_pid)
		{
			mxLogFmt(LOG_DEBUG,"received PF_KEY message is not intended for us\n");
			continue;
		}
		if (msg->sadb_msg_seq != mSequence)
		{
			mxLogFmt(LOG_DEBUG,"received PF_KEY message with unexpected sequence"
							   "number, was %d expected %d\n",msg->sadb_msg_seq,mSequence);

			if (msg->sadb_msg_seq == 0)
			{
				/* FreeBSD and Mac OS X do this for the response to
				 * SADB_X_SPDGET (but not for the response to SADB_GET).
				 * FreeBSD: 'key_spdget' in /usr/src/sys/netipsec/key.c. */
			}
			else if (msg->sadb_msg_seq < mSequence)
			{
				continue;
			}
			else
			{
				mMutex.unlock();
				return STATUS_FAILED;
			}
		}
		if (msg->sadb_msg_type != in->sadb_msg_type)
		{
			mxLogFmt(LOG_DEBUG,"received PF_KEY message of wrong type, "
							   "was %d expected %d, ignoring\n",msg->sadb_msg_type,in->sadb_msg_type);

		}
		break;
	}

	*out_len = len;
	*out = (struct sadb_msg*)malloc(len);
	memcpy(*out, buf, len);

	mMutex.unlock();
	return STATUS_SUCCESS;
}

/**
 * convert a protocol identifier to the PF_KEY sa type
 */
u8_t xfrmKernelPfkey::convertProtoToSaType(u8_t protocol)
{
	switch (protocol)
	{
		case IPPROTO_ESP:
			return SADB_SATYPE_ESP;
		case IPPROTO_AH:
			return SADB_SATYPE_AH;
		case IPPROTO_COMP:
			return SADB_X_SATYPE_IPCOMP;
		default:
			return protocol;
	}

}

void xfrmKernelPfkey::addAddrExt(struct sadb_msg *msg, struct sockaddr *pAddr, u16_t type, u8_t proto, u8_t prefixlen)
{
	struct sadb_address *addr = (struct sadb_address*)PFKEY_EXT_ADD_NEXT(msg);
	size_t len;

	addr->sadb_address_exttype = type;
	addr->sadb_address_proto = proto;
	addr->sadb_address_prefixlen = prefixlen;
	len = convertIpToXfrm(pAddr, (struct sockaddr *)(addr + 1));
	addr->sadb_address_len = PFKEY_LEN(sizeof(*addr) + len);
	PFKEY_EXT_ADD(msg, addr);
}

size_t xfrmKernelPfkey::convertIpToXfrm(struct sockaddr *pAddr, struct sockaddr* xfrm_addr)
{
	if (pAddr->sa_family == AF_INET) {
		struct sockaddr_in *pAddrV4 = (struct sockaddr_in *)pAddr;
		memcpy(xfrm_addr, pAddrV4, sizeof(struct sockaddr_in));
		return sizeof(struct sockaddr_in);
	} else {
		struct sockaddr_in6 *pAddrV6 = (struct sockaddr_in6 *)pAddr;
		memcpy(xfrm_addr, pAddrV6, sizeof(struct sockaddr_in6));
		return sizeof(struct sockaddr_in6);
	}
}


