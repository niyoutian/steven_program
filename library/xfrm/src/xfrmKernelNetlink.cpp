#include <unistd.h>
#include <netinet/in.h>
#include "mxLog.h"
#include "xfrmKernelNetlink.h"


//https://www.cnblogs.com/elewei/p/8097411.html

xfrmKernelNetlink::xfrmKernelNetlink()
{
	mSocket = 0;
	mSocketEvents = 0;
	mSequence = 202;
	mConfigSpiMin = KERNEL_SPI_MIN;
	mConfigSpiMax = KERNEL_SPI_MAX;
}

xfrmKernelNetlink::~xfrmKernelNetlink()
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

u32_t xfrmKernelNetlink::createSocket(u32_t registerEvents)
{
	struct sockaddr_nl addr = {
		.nl_family = AF_NETLINK,
	};

	/* create a AF_NETLINK socket to communicate with the kernel */
	mSocket = socket(AF_NETLINK, SOCK_RAW, NETLINK_XFRM);
	if (mSocket <= 0) {
		mxLogFmt(LOG_DEBUG,"unable to create netlink socket: %s (%d)\n",strerror(errno), errno);
		return STATUS_FAILED;
	}

	if (bind(mSocket, (struct sockaddr*)&addr, sizeof(addr))) {
		mxLogFmt(LOG_DEBUG,"unable to bind netlink socket: %s (%d)\n",strerror(errno), errno);
		return STATUS_FAILED;
	}

	if (registerEvents) {
		/* create a AF_NETLINK socket for ACQUIRE & EXPIRE MIGRATE & MAPPING*/
		mSocketEvents = socket(AF_NETLINK, SOCK_RAW, NETLINK_XFRM);
		if (mSocketEvents <= 0) {
			mxLogFmt(LOG_DEBUG,"unable to create netlink socket: %s (%d)\n",strerror(errno), errno);
			return STATUS_FAILED;
		}
		/* ACQUIRE EXPIRE  MIGRATE  MAPPING*/
		addr.nl_groups = 0xc3;
		if (bind(mSocketEvents, (struct sockaddr*)&addr, sizeof(addr))) {
			mxLogFmt(LOG_DEBUG,"unable to bind netlink socket: %s (%d)\n",strerror(errno), errno);
			return STATUS_FAILED;
		}

	}

	return STATUS_SUCCESS;
}
/* RFC3549
Netlink Message Header
0					1					2					3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|						   Length							  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|			 Type			   |		   Flags			  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|					   Sequence Number						  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|					   Process ID (PID) 					  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Length: 32 bits
  The length of the message in bytes, including the header.
Type: 16 bits
This field describes the message content.
Flags: 16 bits
The standard flag bits used in Netlink are
NLM_F_REQUEST   NLM_F_MULTI     NLM_F_ACK       NLM_F_ECHO      

*/
/**
 * Get an SPI for a specific protocol from the kernel.
 */
u32_t xfrmKernelNetlink::getSpi(struct sockaddr *pSrc, struct sockaddr *pDst, u8_t protocol, u32_t& spi)
{
	unsigned char request[NETLINK_BUFFER_SIZE];
	struct nlmsghdr *hdr, *out;
	struct xfrm_userspi_info *userspi;
	uint32_t received_spi = 0;
	size_t len;

	memset(&request, 0, sizeof(request));

	hdr = (struct nlmsghdr *)request;
	hdr->nlmsg_flags = NLM_F_REQUEST;
	hdr->nlmsg_type = XFRM_MSG_ALLOCSPI;
	hdr->nlmsg_len = NLMSG_LENGTH(sizeof(struct xfrm_userspi_info));
	hdr->nlmsg_seq = mSequence++;
	hdr->nlmsg_pid = getpid();
	

	userspi = (struct xfrm_userspi_info*)NLMSG_DATA(hdr);
	convertIpToXfrm(pSrc,userspi->info.saddr);
	convertIpToXfrm(pDst,userspi->info.id.daddr);
	userspi->info.id.proto = protocol;
	userspi->info.mode = XFRM_MODE_TUNNEL;
	userspi->info.family = pSrc->sa_family;
	userspi->min = mConfigSpiMin;
	userspi->max = mConfigSpiMax;

	if (sendNetlinkSocket(mSocket ,hdr, &out, &len) == STATUS_SUCCESS) {
		hdr = out;
		while (NLMSG_OK(hdr, len)) {
			switch (hdr->nlmsg_type) {
				case XFRM_MSG_NEWSA:
				{
					struct xfrm_usersa_info* usersa = (struct xfrm_usersa_info *)NLMSG_DATA(hdr);
					received_spi = usersa->id.spi;
					mxLogFmt(LOG_DEBUG,"allocating SPI %08x sucess\n",received_spi);
					break;
				}
				case NLMSG_ERROR:
				{
					struct nlmsgerr *err = (struct nlmsgerr *)NLMSG_DATA(hdr);
					mxLogFmt(LOG_DEBUG,"allocating SPI failed: %s (%d)\n",strerror(-err->error), -err->error);
					break;
				}
				case NLMSG_DONE:
					break;

				default:
					hdr = NLMSG_NEXT(hdr, len);
					continue;
			}
			break;
		}
		free(out);
	}
	
	if (received_spi == 0) {
		return STATUS_FAILED;
	}

	spi = received_spi;
	return STATUS_SUCCESS;
}

void xfrmKernelNetlink::convertIpToXfrm(struct sockaddr *pAddr, xfrm_address_t& xfrm_addr)
{
	if (pAddr->sa_family == AF_INET) {
		struct sockaddr_in *pAddrV4 = (struct sockaddr_in *)pAddr;
		memcpy(&xfrm_addr.a4, (const void*)&pAddrV4->sin_addr, 4);
	} else {
		struct sockaddr_in6 *pAddrV6 = (struct sockaddr_in6 *)pAddr;
		memcpy(xfrm_addr.a6, (const void*)&pAddrV6->sin6_addr, sizeof(struct in6_addr));
	}
}

u32_t xfrmKernelNetlink::sendNetlinkSocket(s32_t socket, struct nlmsghdr *in, struct nlmsghdr **out,size_t *out_len)
{
	int in_len, len;
	unsigned char buf[NETLINK_BUFFER_SIZE];

	memset(buf,0,sizeof(buf));
	
	mMutex.lock();
	in_len = in->nlmsg_len;
	
	while (true)
	{
#if 1
		len = send(socket, in, in_len, 0);
#else
		struct sockaddr_nl addr = {
			.nl_family = AF_NETLINK,
		};
		len = sendto(socket, in, in->nlmsg_len, 0,
					 (struct sockaddr*)&addr, sizeof(addr));
#endif

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
		struct nlmsghdr *hdr;

		hdr = (struct nlmsghdr*)buf;
		len = recv(socket, buf, sizeof(buf), MSG_TRUNC);
		
		if (len < 0)
		{
			if (errno == EINTR)
			{
				mxLogFmt(LOG_DEBUG,"got interrupted\n");
				/* interrupted, try again */
				continue;
			}

			mxLogFmt(LOG_DEBUG,"error reading from NETLINK socket: %s\n",strerror(errno));
			mMutex.unlock();
			return STATUS_FAILED;
		}
		if (len < sizeof(struct nlmsghdr) ||
			hdr->nlmsg_len < sizeof(struct nlmsghdr))
		{
			mxLogFmt(LOG_DEBUG,"received corrupted NETLINK message\n");
			mMutex.unlock();
			return STATUS_FAILED;
		}
		break;
	}
	*out_len = len;
	*out = (struct nlmsghdr*)malloc(len);
	memcpy(*out, buf, len);

	mMutex.unlock();
	return STATUS_SUCCESS;

}



