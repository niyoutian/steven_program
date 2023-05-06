#ifndef __MXTIMER_H__
#define __MXTIMER_H__
#include "mxMutex.h"
#include "mxLog.h"

typedef void timerCallBack( void *param );

typedef struct timerPara_s{
	u32_t 	interval;			/**< timer interval ms */

	void * 	para;			//the parameter is callback funtion adddress and para pointer
	timerCallBack* Func;

	s8_t *file;
	s32_t line;
	s8_t *caller;
} timerPara_t;


class mxTimer
{
public:
	mxTimer();
	~mxTimer();	
	virtual u32_t startTimer(void) = 0;
	virtual void stopTimer(void) = 0;
};

#endif