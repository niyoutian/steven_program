#ifndef __MX_HANDLE_PACKET_H__
#define __MX_HANDLE_PACKET_H__
#include <sys/socket.h>  
#include <netinet/in.h> 
#include "mxDef.h"

#define packetLog   mxLogFmt
typedef struct
{
	
} PacketCallbacks_t;

class mxEngineBase;
class mxHandlePacket
{
public:
	mxHandlePacket(mxEngineBase *engine);
	virtual ~mxHandlePacket();
	
	virtual s32_t eventRead(s32_t fd) = 0;
	virtual s32_t eventWrite(s32_t fd) = 0;
	virtual s32_t eventError(s32_t fd) = 0;
	s32_t  createSocket(s32_t family, s32_t type, s32_t proto); 
	s32_t  setOptNonBlocking(bool value);
	s32_t  setOptBufferSize(u32_t rx_value, u32_t tx_value);
	s32_t  bindSocket(struct sockaddr &addr);
	u16_t           mPort;
	mxEngineBase*   mpEngine;
	bool            mServerFlag;

private:
	s32_t           mSocket;
	s32_t           mFamily;
	s32_t 			mType;
	s32_t			mProtocol;
	PacketCallbacks_t	mCallback;
};







#endif
