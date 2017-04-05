#ifndef __MX_HANDLE_UDP_PACKET_H__
#define __MX_HANDLE_UDP_PACKET_H__
#include "mxHandlePacket.h"


class mxHandleUdpPacket : public mxHandlePacket
{
public:
	mxHandleUdpPacket(mxEngineBase *engine);
	~mxHandleUdpPacket();
	s32_t eventRead(s32_t fd);
	s32_t eventWrite(s32_t fd);
	s32_t eventError(s32_t fd);
	s32_t openConnect(s32_t family, s32_t type, s32_t proto, struct sockaddr &addr);
private:
	s32_t serverRead(s32_t fd);
	s32_t clientRead(s32_t fd);
};







#endif
