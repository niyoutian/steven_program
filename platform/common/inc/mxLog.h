#ifndef __MXLOG_H__
#define __MXLOG_H__
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include "mxDef.h"

/*

level
       This determines the importance of the message.  The levels are, in  order  of  decreasing
       importance:
       LOG_EMERG      system is unusable
       LOG_ALERT      action must be taken immediately
       LOG_CRIT       critical conditions
       LOG_ERR        error conditions
       LOG_WARNING    warning conditions
       LOG_NOTICE     normal, but significant, condition
       LOG_INFO       informational message
      
       LOG_DEBUG      debug-level message
       The function setlogmask(3) can be used to restrict logging to specified levels only.

*/
/*use LOG_LOCAL2 to store the log */
#define LOG_FACILITY		LOG_LOCAL2
#define LOG_MAX_DATA        256
class mxLog
{
public:
	mxLog();
	~mxLog();
	static mxLog* getInstance(void);
	void initialize(const s8_t *ident);
	void logFmt(s32_t level,const s8_t* file, const s8_t* function, 
						const s32_t line,const s8_t* format, ...);
private:
	static mxLog* mpInstance;
};

#define mxLogInit(ident) \
		mxLog::getInstance()->initialize(ident)

#define mxLogFmt(level,fmt,...) \
		mxLog::getInstance()->logFmt(level , __FILE__,  __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#endif
