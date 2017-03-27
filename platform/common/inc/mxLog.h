#ifndef __MXLOG_H__
#define __MXLOG_H__
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>

/*use LOG_LOCAL2 to store the log */
#define LOG_FACILITY		LOG_LOCAL2

class mxLog
{
public:
	mxLog();
	~mxLog();
	static mxLog* getInstance(void);
private:
	static mxLog* mpInstance;
};

#endif
