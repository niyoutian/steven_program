#include "test_log.h"
#include "mxLog.h"











int main(int argc, char* argv[])
{
	mxLogInit("Test");
	mxLogFmt(LOG_DEBUG,"steven test");
	return 0;
}

