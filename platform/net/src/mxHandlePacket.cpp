#include "mxHandlePacket.h"


mxHandlePacket::mxHandlePacket(mxEngineBase *engine) : mpEngine(engine),
	mFamily(AF_INET), mPort(0), mServerFlag(false), mSocket(-1)
{
	
}

mxHandlePacket::~mxHandlePacket()
{
	
}


