#include <stdio.h>
#include <unistd.h>
#include "mxLog.h"
#include "mxMiniTimer.h"



void timerCallBack1(void *param)
{
    mxLogFmt(LOG_DEBUG,"timerCallBack\n");
}

void test_timer1(void)
{
    mxLog::getInstance()->initialize(MXLOG_CONSOLE, LOG_DEBUG, "timer");
    mxMiniTimer::getInstance()->startTimer();
    timerPara_t param;
    param.Func = timerCallBack1;
    param.interval = 200;
    mxLogFmt(LOG_DEBUG,"add timer\n");
    mxMiniTimer::getInstance()->addTimer(&param);
    pause();
}


int main(int argc, char* argv[])
{
	test_timer1();

    return 0;
}
