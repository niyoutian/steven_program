#include "mxLog.h"

mxLog* mxLog::mpInstance = NULL;
mxLog::mxLog()
{
	
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

void mxLog::initialize(const s8_t *ident)
{
	openlog(ident,LOG_PID,LOG_LOCAL2);
}

void mxLog::logFmt(s32_t level,const s8_t* file, const s8_t* function, 
						const s32_t line,const s8_t* format, ...)
{
	s8_t  logStr[LOG_MAX_DATA]= {0};
	va_list ap1;
	va_start(ap1, format);
	vsnprintf(logStr, sizeof(logStr), format, ap1);
	syslog(level,"[%s-%s-%d]%s",function,file,line,logStr);
	va_end(ap1);
}

