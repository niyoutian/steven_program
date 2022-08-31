#ifndef __MXLOG_H__
#define __MXLOG_H__
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include "mxDef.h"

enum {
	MXLOG_SYSLOG = (1 << 0),
	MXLOG_STDIO  = (1 << 1)
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
/*use LOG_LOCAL2 to store the log */
#define LOG_FACILITY		LOG_LOCAL2
#define LOG_MAX_DATA        256
#define LOG_LEVEL           LOG_DEBUG
class mxLog
{
public:
	mxLog();
	~mxLog();
	static mxLog* getInstance(void);
	void initialize(u32_t channel, const s8_t *ident);
	void logFmt(s32_t level,const s8_t* file, const s8_t* function, 
						const s32_t line,const s8_t* format, ...);
private:
	static mxLog* mpInstance;
	u32_t  mxLogChannel;
	const s8_t *mxIdent;
};

#define mxLogInit(ident) \
		mxLog::getInstance()->initialize(MXLOG_SYSLOG, ident)

#define mxLogInit2(channel, ident) \
		mxLog::getInstance()->initialize(channel, ident)

#define mxLogFmt(level,fmt,...) \
		mxLog::getInstance()->logFmt(level , __FILE__,  __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#endif
