#include <unistd.h>
#include <fcntl.h>
#include "mxMiniTimer.h"

mxMiniTimer::mxMiniTimer()
{
    mStatus = false;
    mThreadId = 0;
    mActiveTimerCount = 0;
}

mxMiniTimer::~mxMiniTimer()
{

}

mxMiniTimer* mxMiniTimer::mpInstance = NULL;

mxMiniTimer* mxMiniTimer::getInstance(void)
{
	if(mpInstance == NULL)
		mpInstance = new mxMiniTimer();
	return mpInstance;
}


u32_t mxMiniTimer::startTimer(void)
{
	mMutex.lock();
    if ( mThreadId ) {
		mxLogFmt(LOG_ERR, "the timer thread has been started!\n");
        mMutex.unlock();
		return STATUS_FAILED;
	}
    initWakeSignal();
	mStatus = true;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create( &mThreadId, &attr, mainThreadEntry, this );
    mxLogFmt(LOG_ERR, "before wait!\n");
	mMutex.wait();
     mxLogFmt(LOG_ERR, "after wait!\n");
	mMutex.unlock();

    return STATUS_SUCCESS;
}

void* mxMiniTimer::mainThreadEntry(void* param)
{
	mxMiniTimer *pEngine = (mxMiniTimer*) param;
    
    pEngine->runTimerTask();
	return NULL;
}

void mxMiniTimer::processTimeout(struct timeval &time_value)
{
    struct timeval curr_time;
    struct timespec ts;
    miniTimerBlock_t * pTmBlk = NULL;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    curr_time.tv_sec = ts.tv_sec;
    curr_time.tv_usec =  ts.tv_nsec / 1000;

    while(1) {
        mMutex.lock();
        if (mTimerList.empty()) {
            time_value.tv_sec = 2;
            time_value.tv_usec = 0;
            mMutex.unlock();
            break;
        }
        pTmBlk = mTimerList.front();
        if (getTimeDiff(&curr_time, &pTmBlk->time) >= 0) {
			mTimerList.pop_front();
            mMutex.unlock();
            pTmBlk->Func(pTmBlk->para);
		} else {
            
            time_value.tv_sec = pTmBlk->time.tv_sec - curr_time.tv_sec;
            // mxLogFmt(LOG_ERR, "pTmBlk %d %d %d\n",pTmBlk->time.tv_sec, curr_time.tv_sec, time_value.tv_sec);
            // mxLogFmt(LOG_ERR, "pTmBlk2 %d %d \n",pTmBlk->time.tv_usec, curr_time.tv_usec);
            if (pTmBlk->time.tv_usec < curr_time.tv_usec) {
                time_value.tv_sec -= 1;
                time_value.tv_usec = 1000000 + pTmBlk->time.tv_usec - curr_time.tv_usec;

            } else {
                time_value.tv_usec = pTmBlk->time.tv_usec - curr_time.tv_usec;
            }

            // less than 1ms
            if ((time_value.tv_sec == 0) && (time_value.tv_usec < 1000)) {
                time_value.tv_usec = 1000;
            }
            mxLogFmt(LOG_ERR, "processTimeout %d %d\n",time_value.tv_sec, time_value.tv_usec);
            mMutex.unlock();
            break;
        }
    }

    return;
}

//int select(int nfds, fd_set *readfds, fd_set *writefds,fd_set *exceptfds, struct timeval *timeout);
void mxMiniTimer::runTimerTask(void)
{
    struct timeval next_time;
    fd_set read_set;
    int ret = 0;
    char buf[4];

    mxLogFmt(LOG_DEBUG,"start timer task with pthread id=%d\n", mThreadId);

	mMutex.lock();
	mMutex.signal();
	mMutex.unlock();
	
	while(mStatus)
	{
        FD_ZERO(&read_set);
        FD_SET(mWakerPipeRecv, &read_set);
        processTimeout(next_time);
        ret = select(mWakerPipeRecv+1, &read_set, NULL, NULL, &next_time);
        if (ret < 0) {
            mxLogFmt(LOG_DEBUG,"select error ret=%d\n", errno);
            sleep(1);
            continue;
        } else if (ret == 0) {
            mxLogFmt(LOG_DEBUG,"select timeout\n");
            continue;
        }
        if (FD_ISSET(mWakerPipeRecv, &read_set)) {
            while (read(mWakerPipeRecv, buf, 4) > 0);
        }
	}
}


void mxMiniTimer::stopTimer(void)
{
	void *pRet = NULL;
	mStatus= false;
	if(mThreadId) 
	{
		if(pthread_join(mThreadId, &pRet ) != 0) 
		{
			int err = errno;
			mxLogFmt(LOG_ERR, "pthread_join fail. reason %s \n",strerror(err));
		}
		mThreadId = 0;
	}
}

s32_t mxMiniTimer::getTimeDiff(struct timeval *t1, struct timeval *t2)
{
	return (t1->tv_sec - t2->tv_sec) * 1000 + (t1->tv_usec - t2->tv_usec) / 1000;
}

u32_t mxMiniTimer::addToTimerList(miniTimerBlock_t * pTmBlk)
{
    list<miniTimerBlock_t *>::iterator it;
    miniTimerBlock_t *pTmIndex = NULL;

    if (mTimerList.empty()) {
        mTimerList.push_back(pTmBlk);
        return STATUS_WAKE_UP;
    }

    for (it = mTimerList.begin(); it != mTimerList.end(); it++) {
        pTmIndex = *it;
		if (getTimeDiff(&pTmIndex->time, &pTmBlk->time) > 0) {
			break;
		}
    }

    if (it == mTimerList.end()) {
        mTimerList.push_back(pTmBlk);
        return STATUS_SUCCESS;
    }

    mTimerList.insert(it, pTmBlk);

    if (it == mTimerList.begin()) {
        return STATUS_WAKE_UP;
    }
    return STATUS_SUCCESS;
}

u32_t mxMiniTimer::initWakeSignal(void)
{
    int fds[2];
	if (pipe(fds) < 0) {
		return STATUS_FAILED;
    }

	fcntl(fds[0], F_SETFD, fcntl(fds[0], F_GETFD) | FD_CLOEXEC);
	fcntl(fds[0], F_SETFL, fcntl(fds[0], F_GETFL) | O_NONBLOCK);

	fcntl(fds[1], F_SETFD, fcntl(fds[1], F_GETFD) | FD_CLOEXEC);
	fcntl(fds[1], F_SETFL, fcntl(fds[1], F_GETFL) | O_NONBLOCK);

    mWakerPipeSend = fds[1];
    mWakerPipeRecv = fds[0];

    return STATUS_SUCCESS;
}

void mxMiniTimer::sendWakeSignal(void)
{
	do {
		if (write(mWakerPipeSend, "w", 1) < 0) {
			if (errno == EINTR)
				continue;
		}
		break;
	} while (1);
}

 u32_t mxMiniTimer::addTimer( timerPara_t *param )
 {
    u32_t ret = 0;
    miniTimerBlock_t* pTmBlk = NULL;

    pTmBlk = (miniTimerBlock_t*)malloc(sizeof(miniTimerBlock_t));
    if (pTmBlk == NULL) {
        mxLogFmt(LOG_ERR, "alloc timer block failure!\n");
        return STATUS_ERR_MALLOC;
    }
    memset(pTmBlk, 0, sizeof(miniTimerBlock_t));
    mxLogFmt(LOG_DEBUG, "malloc miniTimerBlock_t %p\n",pTmBlk);

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    pTmBlk->time.tv_sec = ts.tv_sec;
    pTmBlk->time.tv_usec =  ts.tv_nsec / 1000;
    pTmBlk->time.tv_sec += param->interval / 1000;
    pTmBlk->time.tv_usec +=  (param->interval % 1000) * 1000;
	if (pTmBlk->time.tv_usec > 1000000) {
		pTmBlk->time.tv_sec++;
		pTmBlk->time.tv_usec -= 1000000;
	}

    pTmBlk->Func= param->Func;
    pTmBlk->para = param->para;
    pTmBlk->file = param->file;
    pTmBlk->line = param->line;
    pTmBlk->caller = param->caller;

    mMutex.lock();
    mActiveTimerCount++;
    pTmBlk->timerId = mActiveTimerCount;
    ret = addToTimerList(pTmBlk);
    mMutex.unlock();
    if (ret == STATUS_WAKE_UP) {
        sendWakeSignal();
    }

 }