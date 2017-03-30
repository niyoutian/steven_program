#ifndef __MX_HANDLE_PACKET_H__
#define __MX_HANDLE_PACKET_H__


class mxEngineBase;
class mxHandlePacket
{
public:
	mxHandlePacket(mxEngineBase *engine);
	virtual ~mxHandlePacket();
	mxEngineBase *mpEngine;
private:
};







#endif
