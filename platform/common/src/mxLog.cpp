#include "mxLog.h"

mxLog* mxLog::mpInstance = NULL;
mxLog::mxLog()
{
	openlog("platform",LOG_PID|LOG_NDELAY,LOG_USER );
}

mxLog::~mxLog()
{
	closelog();
}

mxLog* mxLog::getInstance(void)
{
        if(mpInstance == NULL)
                mpInstance = new mxLog;
        return mpInstance;
}

