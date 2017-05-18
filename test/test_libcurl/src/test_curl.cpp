#include "mxLog.h"

int main(int argc, char* argv[])
{
	mxLogInit("libcurl");
	mxLogFmt(LOG_DEBUG,"libcurl test");
	return 0;
}

