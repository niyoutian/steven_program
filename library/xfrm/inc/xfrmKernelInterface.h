#ifndef __XFRM_KERNEL_INTERFACE_H__
#define __XFRM_KERNEL_INTERFACE_H__
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/types.h>

#include "mxDef.h"
#include "mxStatus.h"


#define PF_KEY_V2		2

#define MAX_RCV_BUFFER      0
/**
 * Default range for SPIs requested from kernels
 */
#define KERNEL_SPI_MIN 0xc0000000
#define KERNEL_SPI_MAX 0xcfffffff


class xfrmKernelInterface
{
public: 

	virtual u32_t createSocket(u32_t registerEvents) = 0;
	virtual u32_t getSpi(struct sockaddr *pSrc, struct sockaddr *pDst, u8_t protocol, u32_t& spi) = 0;
private:
};


#endif

