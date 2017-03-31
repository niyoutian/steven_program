#ifndef __MX_HANDLE_PACKET_H__
#define __MX_HANDLE_PACKET_H__
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
	
	mxEngineBase *mpEngine;

private:
};







#endif
