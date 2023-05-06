#ifndef __MX_MINI_TIMER_H__
#define __MX_MINI_TIMER_H__
#include <list>
#include "mxDef.h"
#include "mxStatus.h"
#include "mxMutex.h"
#include "mxTimer.h"


typedef struct miniTimerBlock {
    u32_t timerId;         /**< timer id return to user */
    struct timeval time;   /**< timeout value */

	timerCallBack *Func;    /**< timer callback function */
	void  *para;			/**< timer parameter */

	s8_t *file;
	s32_t line;
	s8_t *caller;
} miniTimerBlock_t;

class mxMiniTimer : public mxTimer
{
public:
	mxMiniTimer();
	~mxMiniTimer();	
    static mxMiniTimer* getInstance(void);

    u32_t startTimer(void);
    void stopTimer(void);

    u32_t addTimer( timerPara_t *param );
private:
    static void* mainThreadEntry(void* param);
    void runTimerTask(void);
    u32_t addToTimerList(miniTimerBlock_t * pTmBlk);
    s32_t getTimeDiff(struct timeval *t1, struct timeval *t2);
    u32_t initWakeSignal(void);
    void sendWakeSignal(void);
    void processTimeout(struct timeval &time_value);

    static mxMiniTimer*	 	mpInstance;
    mxMonitoredMutex  		mMutex;
    pthread_t          		mThreadId;
    s32_t                   mWakerPipeSend;
    s32_t                   mWakerPipeRecv;

    bool              	    mStatus;
    u32_t 			    	mActiveTimerCount;

    list<miniTimerBlock_t *> mTimerList;
};


#endif