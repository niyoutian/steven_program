#include <stdio.h>
#include <unistd.h>
#include "mxLog.h"
#include "mxDbusMgr.h"



void timerCallBack1(void *param)
{
    mxLogFmt(LOG_DEBUG,"timerCallBack\n");
}

void test_mydbus1(void)
{
    mxLog::getInstance()->initialize(MXLOG_CONSOLE, LOG_DEBUG, "timer");

    mxDbusMgr *dbus = new mxDbusMgr();
    dbus->initDbus();
    dbus->addDbusWatch("type='signal',interface='org.freedesktop.DBus',member='NameOwnerChanged'");
    dbus->startDbusService();
    pause();
}


int main(int argc, char* argv[])
{
	test_mydbus1();

    return 0;
}