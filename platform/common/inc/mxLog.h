#ifndef __MXLOG_H__
#define __MXLOG_H__
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include "mxDef.h"
#include "mxStatus.h"

#define LOG_FILE_NAME(x) strrchr(x,'/')?strrchr(x,'/')+1:x
/*use LOG_LOCAL2 to store the log */
#define LOG_FACILITY		LOG_LOCAL2
#define LOG_MAX_DATA        256
#define LOG_CONSOLE_COLOUR   1

enum {
	MXLOG_SYSLOG   = (1 << 0),
	MXLOG_CONSOLE  = (1 << 1),
	MXLOG_NONE     = (1 << 2)
};


/*

level
       This determines the importance of the message.  The levels are, in  order  of  decreasing
       importance:
       LOG_EMERG      0  system is unusable
       LOG_ALERT      1  action must be taken immediately
       LOG_CRIT       2  critical conditions
       LOG_ERR        3  error conditions
       LOG_WARNING    4  warning conditions
       LOG_NOTICE     5  normal, but significant, condition
       LOG_INFO       6  informational message
       LOG_DEBUG      7  debug-level message
       The function setlogmask(3) can be used to restrict logging to specified levels only.

*/

class mxLog
{
public:
	mxLog();
	~mxLog();
	static mxLog* getInstance(void);
	u32_t initialize(u32_t channel, u32_t level, const s8_t *ident);
	void logFmt(s32_t level,const s8_t* file, const s8_t* function, 
						const s32_t line,const s8_t* format, ...);
private:
	const char *getLevelString(u32_t level);
	void openConsoleColour(u32_t level);
	void closeConsoleColour(void);


	static mxLog* mpInstance;
	u32_t  mxLogChannel;
	u32_t  mxLogLevel;
	const s8_t *mxIdent;
};

#define mxLogInit(ident) \
		mxLog::getInstance()->initialize(MXLOG_SYSLOG, LOG_DEBUG, ident)

#define mxLogFmt(level,fmt,...) \
		mxLog::getInstance()->logFmt(level , __FILE__,  __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#endif
