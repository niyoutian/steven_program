#include "mxMutex.h"




mxRwLock::mxRwLock()
{
	pthread_rwlock_init(&mRwLock, NULL);
}

mxRwLock::~mxRwLock()
{
	pthread_rwlock_destroy(&mRwLock);
}

void mxRwLock::rlock(s32_t millsec)
{
	if (millsec == 0)
	{
		pthread_rwlock_rdlock(&mRwLock);
	}
	else
	{
		struct timespec ts;
		ts.tv_sec = millsec / 1000;
		ts.tv_nsec = (millsec % 1000) * 10^6;
		pthread_rwlock_timedrdlock(&mRwLock, &ts);
	}	
}

void mxRwLock::wlock(s32_t millsec)
{
	if (millsec == 0)
	{
		pthread_rwlock_wrlock(&mRwLock);
	}
	else
	{
		struct timespec ts;
		ts.tv_sec = millsec / 1000;
		ts.tv_nsec = (millsec % 1000) * 10^6;
		pthread_rwlock_timedwrlock(&mRwLock, &ts);
	}	
}

void mxRwLock::unlock(void)
{
	pthread_rwlock_unlock(&mRwLock);
}


/*
互斥锁创建
有两种方法创建互斥锁，静态方式和动态方式。
静态方式:
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
在LinuxThreads实现中，pthread_mutex_t是一个结构，而PTHREAD_MUTEX_INITIALIZER则是一个结构常量。

动态方式:
采用pthread_mutex_init()函数来初始化互斥锁，API定义如下：
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr)
其中mutexattr用于指定互斥锁属性，如果为NULL则使用缺省属性。
*/
mxMonitoredMutex::mxMonitoredMutex(bool locked)
{
	pthread_mutex_init(&mMutex, NULL);
	pthread_cond_init(&mCond, NULL);	
	if (locked)
		lock();
}

/*
pthread_mutex_destroy()用于注销一个互斥锁，API定义如下：
int pthread_mutex_destroy(pthread_mutex_t *mutex)
销毁一个互斥锁即意味着释放它所占用的资源，且要求锁当前处于开放状态。
由于在Linux中，互斥锁并不占用任何资源，因此LinuxThreads中的 pthread_mutex_destroy()
除了检查锁状态以外（锁定状态则返回EBUSY）没有其他动作。
*/
mxMonitoredMutex::~mxMonitoredMutex()
{
	pthread_mutex_destroy(&mMutex);
	pthread_cond_destroy(&mCond);	
}

void mxMonitoredMutex::set(void)
{
	pthread_cond_broadcast(&mCond);
}

/*
http://www.manongjc.com/article/98450.html

*/
void mxMonitoredMutex::signal(void)
{
	pthread_cond_signal(&mCond);
}


void mxMonitoredMutex::wait(void)
{
	pthread_cond_wait(&mCond, &mMutex);
}

bool mxMonitoredMutex::isLocked(void)
{
	s32_t tryLock = pthread_mutex_trylock(&mMutex);
	if (tryLock== EBUSY)
		return true;
	else
		return false;
}

void mxMonitoredMutex::unlock(void)
{
	pthread_mutex_unlock(&mMutex);
}

void mxMonitoredMutex::lock(void)
{
	pthread_mutex_lock(&mMutex);
}

s32_t mxMonitoredMutex::waitfor(const struct timespec *abstime) 
{
	return pthread_cond_timedwait(&mCond, &mMutex, abstime);
}

