#include "mxHandleUdpPacket.h"
#include "mxEngineBase.h"

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

s32_t mxHandleUdpPacket::setOptAuxiliaryData(bool value)
{
	s32_t opt   = value;
	s32_t level = IPPROTO_IP;
	s32_t optname = IP_PKTINFO;
	 if(getFamily() == AF_INET6)
	 {
	        level   =  IPPROTO_IPV6;
	        optname =  IPV6_RECVPKTINFO;	 	
	 }
	 if(setsockopt(getSocket(), level, optname, &opt, sizeof(opt)) < 0)
	 {
	 	packetLog(LOG_ERR,"set option failed");
		return -1;
	 }
	 return 0;
}

s32_t mxHandleUdpPacket::openConnect(s32_t family, s32_t type, s32_t proto, struct sockaddr &addr)
{
	if(createSocket(family,type,proto))
		return -1;
	if(setOptNonBlocking(true))
		return -1;
	if(bindSocket(addr))
		return -1;
	mServerFlag = true;
	mpEngine->addEvent(getSocket(),EVENT_READ);
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



