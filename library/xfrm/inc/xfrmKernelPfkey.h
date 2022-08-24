#ifndef __XFRM_KERNEL_PFKEY_H__
#define __XFRM_KERNEL_PFKEY_H__
#include <string.h>
#include "mxLog.h"
#include "mxMutex.h"

#include "xfrmKernelInterface.h"

/** buffer size for PF_KEY messages */
#define PFKEY_BUFFER_SIZE 4096
/** PF_KEY messages are 64 bit aligned */
#define PFKEY_ALIGNMENT 8
/** aligns len to 64 bits */
#define PFKEY_ALIGN(len) (((len) + PFKEY_ALIGNMENT - 1) & ~(PFKEY_ALIGNMENT - 1))
/** calculates the properly padded length in 64 bit chunks */
#define PFKEY_LEN(len) ((PFKEY_ALIGN(len) / PFKEY_ALIGNMENT))
/** calculates user mode length i.e. in bytes */
#define PFKEY_USER_LEN(len) ((len) * PFKEY_ALIGNMENT)

/** given a PF_KEY message header and an extension this updates the length in the header */
#define PFKEY_EXT_ADD(msg, ext) ((msg)->sadb_msg_len += ((struct sadb_ext*)ext)->sadb_ext_len)
/** given a PF_KEY message header this returns a pointer to the next extension */
#define PFKEY_EXT_ADD_NEXT(msg) ((struct sadb_ext*)(((char*)(msg)) + PFKEY_USER_LEN((msg)->sadb_msg_len)))
/** copy an extension and append it to a PF_KEY message */
#define PFKEY_EXT_COPY(msg, ext) (PFKEY_EXT_ADD(msg, memcpy(PFKEY_EXT_ADD_NEXT(msg), ext, PFKEY_USER_LEN(((struct sadb_ext*)ext)->sadb_ext_len))))
/** given a PF_KEY extension this returns a pointer to the next extension */
#define PFKEY_EXT_NEXT(ext) ((struct sadb_ext*)(((char*)(ext)) + PFKEY_USER_LEN(((struct sadb_ext*)ext)->sadb_ext_len)))
/** given a PF_KEY extension this returns a pointer to the next extension also updates len (len in 64 bit words) */
#define PFKEY_EXT_NEXT_LEN(ext,len) ((len) -= (ext)->sadb_ext_len, PFKEY_EXT_NEXT(ext))
/** true if ext has a valid length and len is large enough to contain ext (assuming len in 64 bit words) */
#define PFKEY_EXT_OK(ext,len) ((len) >= PFKEY_LEN(sizeof(struct sadb_ext)) && \
				(ext)->sadb_ext_len >= PFKEY_LEN(sizeof(struct sadb_ext)) && \
				(ext)->sadb_ext_len <= (len))


class xfrmKernelPfkey : public xfrmKernelInterface
{
public: 
	xfrmKernelPfkey();
	~xfrmKernelPfkey();

	u32_t createSocket(u32_t registerEvents);
	u32_t getSpi(struct sockaddr *pSrc, struct sockaddr *pDst, u8_t protocol, u32_t& spi);
	
	mxMonitoredMutex	mMutex;
private:
	u32_t RegisterPfkeySocket(u8_t sa_type);
	u32_t sendPfkeySocket(s32_t socket,struct sadb_msg *in, struct sadb_msg **out, size_t *out_len);
	u8_t convertProtoToSaType(u8_t protocol);
	size_t convertIpToXfrm(struct sockaddr *pAddr, struct sockaddr* xfrm_addr);
	void addAddrExt(struct sadb_msg *msg, struct sockaddr *pAddr, u16_t type, u8_t proto, u8_t prefixlen);
	
	s32_t mSocket; /*PF_KEY socket to communicate with the kernel*/
	s32_t mSocketEvents; /*PF_KEY socket to receive acquire and expire events*/
	u32_t mSequence;    /*sequence number for messages sent to the kernel*/
	
	u32_t mConfigSpiMin; /* 从配置文件读取的 SPI 最小值 */
	u32_t mConfigSpiMax; /* 从配置文件读取的 SPI 最大值 */

};


#endif

