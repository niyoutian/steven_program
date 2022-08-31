#include "mxLog.h"

mxLog* mxLog::mpInstance = NULL;
mxLog::mxLog()
{
	mxLogChannel = MXLOG_SYSLOG;
	mxIdent = NULL;
}

mxLog::~mxLog()
{
	mxLogChannel = 0;
	mxIdent = NULL;
	if (mxLogChannel & MXLOG_SYSLOG) {
		closelog();
	}
}

mxLog* mxLog::getInstance(void)
{
	if(mpInstance == NULL) {
		mpInstance = new mxLog();
	}
	return mpInstance;
}
/*
ident主要被用于加在日志消息中，一般是用某个程序名来代替。如cron, inetd等。

*/
void mxLog::initialize(u32_t channel, const s8_t *ident)
{
	mxLogChannel = channel;
	mxIdent = ident;
	if (mxLogChannel & MXLOG_SYSLOG) {
		openlog(ident,LOG_PID,LOG_LOCAL2);
	}
}

void mxLog::logFmt(s32_t level,const s8_t* file, const s8_t* function, 
						const s32_t line,const s8_t* format, ...)
{
	s8_t  logStr[LOG_MAX_DATA]= {0};
	va_list ap1;

	if (level > LOG_LEVEL) {
		return;
	}
		
	va_start(ap1, format);
	vsnprintf(logStr, sizeof(logStr), format, ap1);

	if (mxLogChannel & MXLOG_STDIO) {
		printf("%s:[%s-%s-%d]%s\n",mxIdent,function,file,line,logStr);
	}

	if (mxLogChannel & MXLOG_SYSLOG) {
		syslog(level,"[%s-%s-%d]%s",function,file,line,logStr);
	}
	
	va_end(ap1);
}

