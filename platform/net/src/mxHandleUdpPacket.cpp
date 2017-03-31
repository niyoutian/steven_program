#include "mxHandleUdpPacket.h"


mxHandleUdpPacket::mxHandleUdpPacket(mxEngineBase *engine) : mxHandlePacket(engine)
{
	
}

mxHandleUdpPacket::~mxHandleUdpPacket()
{
	
}

s32_t mxHandleUdpPacket::eventRead(s32_t fd)
{
	if(mServerFlag)
		return serverRead(fd);
	else
		return clientRead(fd);
}

s32_t mxHandleUdpPacket::eventWrite(s32_t fd)
{
	return 0;
}

s32_t mxHandleUdpPacket::eventError(s32_t fd)
{
	return 0;
}

s32_t mxHandleUdpPacket::serverRead(s32_t fd)
{
	return 0;
}

s32_t mxHandleUdpPacket::clientRead(s32_t fd)
{
	return 0;
}



