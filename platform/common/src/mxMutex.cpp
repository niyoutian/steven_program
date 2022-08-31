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
����������
�����ַ�����������������̬��ʽ�Ͷ�̬��ʽ��
��̬��ʽ:
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
��LinuxThreadsʵ���У�pthread_mutex_t��һ���ṹ����PTHREAD_MUTEX_INITIALIZER����һ���ṹ������

��̬��ʽ:
����pthread_mutex_init()��������ʼ����������API�������£�
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr)
����mutexattr����ָ�����������ԣ����ΪNULL��ʹ��ȱʡ���ԡ�
*/
mxMonitoredMutex::mxMonitoredMutex(bool locked)
{
	pthread_mutex_init(&mMutex, NULL);
	pthread_cond_init(&mCond, NULL);	
	if (locked)
		lock();
}

/*
pthread_mutex_destroy()����ע��һ����������API�������£�
int pthread_mutex_destroy(pthread_mutex_t *mutex)
����һ������������ζ���ͷ�����ռ�õ���Դ����Ҫ������ǰ���ڿ���״̬��
������Linux�У�����������ռ���κ���Դ�����LinuxThreads�е� pthread_mutex_destroy()
���˼����״̬���⣨����״̬�򷵻�EBUSY��û������������
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

