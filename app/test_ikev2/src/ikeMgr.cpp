#include "test_net.h"

#include "mxLog.h"
#include "mxEngineEpoll.h"
#include "mxHandleUdpPacket.h"










int main(int argc, char* argv[])
{
	PacketCallbacks_t cb = {0};
	mxLogInit("net");
	mxLogFmt(LOG_DEBUG,"net test");
	mxEngineBase * engine = new mxEngineEpoll();
	mxLogFmt(LOG_DEBUG,"engine %p",engine);
	mxHandleUdpPacket *handle = new mxHandleUdpPacket(engine);
	handle->registerCallBack(cb);
	mxLogFmt(LOG_DEBUG,"handle %p",handle);
	return 0;
}

