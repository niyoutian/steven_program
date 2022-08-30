#ifndef __MX_MUTEX_H__
#define __MX_MUTEX_H__
#include <pthread.h>
#include "mxDef.h"


class mxRwLock
{
public:
	mxRwLock();
	~mxRwLock();
	void rlock(s32_t millsec = 0);
	void wlock(s32_t millsec = 0);
	void unlock(void);
private:
	pthread_rwlock_t 	mRwLock;
};


class mxMonitoredMutex
{
public:
	mxMonitoredMutex(bool locked = false);
	~mxMonitoredMutex();
	void set(void);
	void signal(void);
	void wait(void);
	bool isLocked(void);
	void unlock(void);
	void lock(void);
	s32_t waitfor(const struct timespec *abstime);
private:	
	pthread_mutex_t mMutex;
	pthread_cond_t	   mCond;
};








#endif