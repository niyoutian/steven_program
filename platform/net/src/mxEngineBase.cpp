#include "mxEngineBase.h"
#include "mxLog.h"


mxEngineBase::mxEngineBase() : mEngineStatus(ES_SHUTDOWN),mThreadId(0),mpHandle(NULL)
{

	
}

mxEngineBase::~mxEngineBase()
{
	stopEngine();	
}

s32_t mxEngineBase::startEngine(void)
{
	if(ES_SHUTDOWN != mEngineStatus)
	{
		engineLog(LOG_ERR,"start engine failed");
		return -1;
	}
	mMutex.lock();
	pthread_create(&mThreadId, NULL, mainThreadEntry, this);
	mMutex.wait();
	mMutex.unlock();
	engineLog(LOG_DEBUG,"start engine thread %u successfully",mThreadId);
	return 0;
}

s32_t mxEngineBase::stopEngine(void)
{
	stopService();
	pthread_join(mThreadId, NULL);
	engineLog(LOG_INFO, "stop engine thread %u successfully\n", mThreadId);	
	return 0;
}

 void* mxEngineBase::mainThreadEntry(void* engine)
 {
 	((mxEngineBase*)engine)->mainThread();
	return NULL;
 }

 void mxEngineBase::mainThread(void)
 {
 	mMutex.lock();
	mEngineStatus = ES_READY;
	mMutex.signal();
	mMutex.unlock();
	service(0);
 }