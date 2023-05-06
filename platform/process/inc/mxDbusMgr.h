#ifndef __MX_DBUS_MGR_H__
#define __MX_DBUS_MGR_H__
#include <list>
#include <dbus/dbus.h>
#include "mxDef.h"
#include "mxMutex.h"
#include "mxStatus.h"

using namespace std;

typedef struct dbus_method_s
{
	string path;
	string inter;
	string method;
} dbus_method_t;

class mxDbusMgr
{
public:
	mxDbusMgr();
	~mxDbusMgr();
    u32_t initDbus(char *req_name = NULL);
    void  deInitDbus(void);
    u32_t startDbusService(void);
    u32_t registerDbusMethod(dbus_method_t *method);
    u32_t addDbusWatch(const char *rule);
    u32_t sendDbusData(char *name, char *path, char *inter, char *method, u8_t data, u32_t len);

private:
    static void* mainThreadEntry(void* param);
    void runDbusTask(void);
    DBusConnection *mpConnect;
    string mNniqueName;
    string mWellKnownName;

    mxMonitoredMutex  		mMutex;
    pthread_t          		mThreadId;
    bool              	    mStatus;

    list <dbus_method_t *> mMethodList;  /**< 记录方法集 */
};


#endif