#ifndef __MX_ENGINE_BASE_H__
#define __MX_ENGINE_BASE_H__
#include "mxDef.h"
#include "mxLog.h"
#include "mxMutex.h"

#define engineLog   mxLogFmt

typedef enum 
{
	ES_SHUTDOWN = 0,
	ES_READY         = 1,
	ES_BUSY            = 2
}EngineStatus_e;

class mxHandlePacket;
class mxEngineBase
{
public:
	mxEngineBase();
	virtual ~mxEngineBase();
	s32_t startEngine(void);
	s32_t stopEngine(void);
	inline bool isOnService() { return (mEngineStatus != ES_SHUTDOWN); }
	virtual s32_t service(s32_t timeout) = 0;
	mxMonitoredMutex	mMutex;
	mxHandlePacket *mpHandle;
private:
	
	static void* mainThreadEntry(void* engine);
	void mainThread(void);
	EngineStatus_e      mEngineStatus;
	pthread_t               mThreadId;
};








#endif