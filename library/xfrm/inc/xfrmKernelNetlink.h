#ifndef __XFRM_KERNEL_NETLINK_H__
#define __XFRM_KERNEL_NETLINK_H__
#include <string.h>
#include <linux/ipsec.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/xfrm.h>

#include <sys/socket.h>

#include "mxLog.h"
#include "mxMutex.h"

#include "xfrmKernelInterface.h"


/** buffer size for NETLINK messages */
#define NETLINK_BUFFER_SIZE 4096



class xfrmKernelNetlink : public xfrmKernelInterface
{
public: 
	xfrmKernelNetlink();
	~xfrmKernelNetlink();

	u32_t createSocket(u32_t registerEvents);
	u32_t getSpi(struct sockaddr *pSrc, struct sockaddr *pDst, u8_t protocol, u32_t& spi);

	mxMonitoredMutex	mMutex;
private:

	void convertIpToXfrm(struct sockaddr *pAddr, xfrm_address_t& xfrm_addr);
	u32_t sendNetlinkSocket(s32_t socket, struct nlmsghdr *in, struct nlmsghdr **out,size_t *out_len);
	
	s32_t mSocket; /*PF_KEY socket to communicate with the kernel*/
	s32_t mSocketEvents; /*PF_KEY socket to receive acquire and expire events*/
	u32_t mSequence;	/*sequence number for messages sent to the kernel*/

	u32_t mConfigSpiMin; /* 从配置文件读取的 SPI 最小值 */
	u32_t mConfigSpiMax; /* 从配置文件读取的 SPI 最大值 */

};


#endif

