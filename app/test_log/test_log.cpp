#include "test_log.h"
#include "mxLog.h"











int main(int argc, char* argv[])
{
	//mxLogInit("Test");
	mxLog::getInstance()->initialize(MXLOG_CONSOLE, LOG_DEBUG, "Text");
	mxLogFmt(LOG_ERR,"steven test error");
	mxLogFmt(LOG_WARNING,"steven test warning");
	mxLogFmt(LOG_NOTICE,"steven test notice");
	mxLogFmt(LOG_INFO,"steven test info");
	mxLogFmt(LOG_DEBUG,"steven test debug");
	return 0;
}

