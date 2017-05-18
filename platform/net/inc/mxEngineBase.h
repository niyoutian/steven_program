#ifndef __MX_ENGINE_BASE_H__
#define __MX_ENGINE_BASE_H__
#include "mxDef.h"
#include "mxLog.h"
#include "mxMutex.h"


#define EVENT_READ		0x01
#define EVENT_WRITE		0x02
#define engineLog   mxLogFmt

typedef enum 
{
	ES_SHUTDOWN     = 0,	// initializing or shutdown
	ES_READY        = 1,	// engine running
	ES_BUSY         = 2		// running but reached capacity, no new connetions 
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
	inline void stopService() { mEngineStatus = ES_SHUTDOWN; }
	inline bool verifyEngineThread() { return (pthread_equal(pthread_self(), mThreadId) == 0); }
	inline pthread_t getEngineThread() { return mThreadId; }
	virtual s32_t service(s32_t timeout) = 0;
	virtual s32_t addEvent(s32_t fd, u8_t event){return 0;}
	virtual s32_t modEvent(s32_t fd, u8_t event){return 0;}
	virtual s32_t remEvent(s32_t fd){return 0;}
	
	mxMonitoredMutex	mMutex;
	mxHandlePacket*     mpHandle;
private:
	
	static void* mainThreadEntry(void* engine);
	void mainThread(void);
	EngineStatus_e      mEngineStatus;
	pthread_t           mThreadId;
};








#endif