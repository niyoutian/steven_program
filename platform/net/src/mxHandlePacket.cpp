#include <fcntl.h>
#include "mxLog.h"
#include "mxEngineBase.h"
#include "mxHandlePacket.h"


mxHandlePacket::mxHandlePacket(mxEngineBase *engine) : mpEngine(engine),
	mFamily(AF_INET), mPort(0), mServerFlag(false), mSocket(-1)
{
	
}

mxHandlePacket::~mxHandlePacket()
{
	
}

s32_t  mxHandlePacket::createSocket(s32_t family, s32_t type, s32_t proto)
{
	if((mSocket = socket(family, type, proto)) < 0)
	{
		packetLog(LOG_ERR,"create socket failed");
		return -1;
	}
	mFamily = family;
	mType = type;
	mProtocol = proto;
	return 0;
}

s32_t  mxHandlePacket::setOptNonBlocking(bool value)
{
	s32_t flags;
	if ((flags = fcntl(mSocket, F_GETFL, 0)) < 0)
	{
		packetLog(LOG_ERR,"get option failed");
		return -1;
	}
	if (value)
	{
		flags |= O_NONBLOCK;
	}
	else
	{
		flags &= (~O_NONBLOCK);
	}
	if (fcntl(mSocket, F_SETFL, flags) < 0)
	{
		packetLog(LOG_ERR,"set option non blocking failed");
		return -1;
	}
	return 0;
}

s32_t  mxHandlePacket::setOptReuseAddr(bool value)
{
	s32_t flag = (value ? 1 : 0);
	if (setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag)) < 0)
	{
		packetLog(LOG_ERR,"set option reuse addr failed");
		return -1;
	}
	return 0;
}


s32_t  mxHandlePacket::setOptBufferSize(u32_t rx_value, u32_t tx_value)
{
	if (tx_value != 0)
		if (setsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, (char *)&tx_value, sizeof(tx_value)) < 0)
		{
			packetLog(LOG_ERR,"set option SO_SNDBUF failed");
			return -1;
		}
	if (rx_value!=0)
		if (setsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, (char *)&rx_value, sizeof(rx_value)) < 0)
		{
			packetLog(LOG_ERR,"set option SO_RCVBUF failed");
			return -1;
		}
    s32_t tx = 0,rx = 0;
    socklen_t sl = sizeof(s32_t);
    if(getsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, (char*)&tx, &sl) < 0) 
    {
    	packetLog(LOG_ERR,"get option SO_SNDBUF failed");
		return -1;
    }
    if(getsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, (char*)&rx, &sl) < 0) 
    {
    	packetLog(LOG_ERR,"get option SO_RCVBUF failed");
		return -1;
    }
	packetLog(LOG_INFO,"get option SO_RCVBUF %d SO_SNDBUF %d",rx,tx);
	return 0;
}

s32_t  mxHandlePacket::bindSocket(struct sockaddr &addr)
{
	if (bind(mSocket,&addr, sizeof(struct sockaddr)) < 0)
	{
		packetLog(LOG_ERR,"bind socket %d failed",mSocket);
		return -1;
	}
	packetLog(LOG_INFO,"bind socket %d successfully",mSocket);
	return 0;
}

s32_t  mxHandlePacket::callListenInd(void)
{
	return 0;
}


