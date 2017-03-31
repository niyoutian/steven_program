#ifndef __MX_HANDLE_PACKET_H__
#define __MX_HANDLE_PACKET_H__
#include <sys/socket.h>  
#include <netinet/in.h> 
#include "mxDef.h"

class mxEngineBase;
class mxHandlePacket
{
public:
	mxHandlePacket(mxEngineBase *engine);
	virtual ~mxHandlePacket();
	
	virtual s32_t eventRead(s32_t fd) = 0;
	virtual s32_t eventWrite(s32_t fd) = 0;
	virtual s32_t eventError(s32_t fd) = 0;

	s32_t           mFamily;
	s32_t 			mType;
	s32_t			mProtocol;
	u16_t           mPort;
	mxEngineBase*   mpEngine;
	bool            mServerFlag;
private:
	s32_t           mSocket;
};







#endif
