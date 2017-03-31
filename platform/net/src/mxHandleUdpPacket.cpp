#include "mxHandleUdpPacket.h"


mxHandleUdpPacket::mxHandleUdpPacket(mxEngineBase *engine) : mxHandlePacket(engine)
{
	
}

mxHandleUdpPacket::~mxHandleUdpPacket()
{
	
}

s32_t mxHandleUdpPacket::eventRead(s32_t fd)
{
	return 0;
}

s32_t mxHandleUdpPacket::eventWrite(s32_t fd)
{
	return 0;
}

s32_t mxHandleUdpPacket::eventError(s32_t fd)
{
	return 0;
}

