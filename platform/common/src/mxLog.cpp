#include <time.h>
#include <sys/time.h>
#include "mxLog.h"


mxLog* mxLog::mpInstance = NULL;
mxLog::mxLog()
{
	mxLogChannel = MXLOG_SYSLOG;
	mxIdent = NULL;
}

mxLog::~mxLog()
{
	if (mxLogChannel & MXLOG_SYSLOG) {
		closelog();
	}

	mxLogChannel = 0;
	mxLogLevel = 0;
	mxIdent = NULL;
}

mxLog* mxLog::getInstance(void)
{
	if(mpInstance == NULL) {
		mpInstance = new mxLog();
	}
	return mpInstance;
}
/*
ident��Ҫ�����ڼ�����־��Ϣ�У�һ������ĳ�������������档��cron, inetd�ȡ�

*/
u32_t mxLog::initialize(u32_t channel, u32_t level, const s8_t *ident)
{
	if ((channel != MXLOG_SYSLOG) && (channel != MXLOG_CONSOLE) && (channel != MXLOG_NONE)) {
		return STATUS_ERR_PARAMETER;
	}
	if (level > LOG_DEBUG) {
		return STATUS_ERR_PARAMETER;
	}
	mxLogChannel = channel;
	mxLogLevel = level;
	mxIdent = ident;
	if (mxLogChannel & MXLOG_SYSLOG) {
		openlog(ident,LOG_PID,LOG_LOCAL2);
	}

	return STATUS_SUCCESS;
}

void mxLog::logFmt(s32_t level,const s8_t* file, const s8_t* function, 
						const s32_t line,const s8_t* format, ...)
{
	s8_t  logStr[LOG_MAX_DATA]= {0};
	va_list ap1;

	if (level > mxLogLevel) {
		return;
	}
		
	va_start(ap1, format);
	vsnprintf(logStr, sizeof(logStr), format, ap1);

	if (mxLogChannel & MXLOG_CONSOLE) {
		struct timeval tv;
		struct tm *newtime;
		time_t current_time;
		char log_time[128];

		time(&current_time);
		newtime = localtime(&current_time);
		strftime(log_time, 128, "%Y-%m-%d %H:%M:%S", newtime);
		gettimeofday(&tv, 0);


		openConsoleColour(level);
		printf("%s.%06d %s<%s>:[%s-%s-%d]%s\n", log_time, (int)(tv.tv_usec), 
				mxIdent, getLevelString(level), function, LOG_FILE_NAME(file), line,logStr);
		closeConsoleColour();
	}

	if (mxLogChannel & MXLOG_SYSLOG) {
		syslog(level,"[%s-%s-%d]%s", function, LOG_FILE_NAME(file), line, logStr);
	}
	
	va_end(ap1);
}

const char *mxLog::getLevelString(u32_t level)
{
	switch(level) {
		case LOG_EMERG:
			return "EMERG";
		case LOG_ALERT:
			return "ALERT";
		case LOG_CRIT:
			return "CRIT";
		case LOG_ERR:
			return "ERR";
		case LOG_WARNING:
			return "WARNING";
		case LOG_NOTICE:
			return "NOTICE";
		case LOG_INFO:
			return "INFO";
		case LOG_DEBUG:
			return "DEBUG";
		default:
			return "NONE";
	}
}

/*
black = "\033[00;30m";
red = "\033[00;31m";
green = "\033[00;32m";
yellow = "\033[00;33m";
blue = "\033[00;34m";
meganta = "\033[00;35m";
cyan = "\033[00;36m";
white = "\033[00;37m";
*/
void mxLog::openConsoleColour(u32_t level)
{
#if LOG_CONSOLE_COLOUR
	switch(level) {
		case LOG_EMERG:
			printf("\033[00;35m");  //meganta
			break;
		case LOG_ALERT:
			printf("\033[00;35m");  //meganta
			break;
		case LOG_CRIT:
			printf("\033[00;35m");  //meganta
			break;
		case LOG_ERR:
			printf("\033[00;31m");   //red
			break;
		case LOG_WARNING:
			printf("\033[00;33m");    //yellow
			break;
		default:
			printf("\033[00;37m");  //white
			break;
	}
#endif
}

void mxLog::closeConsoleColour(void)
{
#if LOG_CONSOLE_COLOUR
	printf("\x1b[0m");
#endif
}
