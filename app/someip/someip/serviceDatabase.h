#ifndef __SERVICE_DATABASE_H__
#define __SERVICE_DATABASE_H__
#include <list>
#include "mxDef.h"
#include "mxMutex.h"
#include "service_interface.h"

using namespace std;

class serviceDatabase
{
public:
	serviceDatabase();
	~serviceDatabase();
    u32_t addEntry(someip_service_cfg_t *service);
    // u32_t updateEntry(struct offerservice_entry *service);
private:
    mxMonitoredMutex	mServiceListMutex; 
    list <someip_service_entry_t *> mServiceList;  /**< 记录对端提供的Service */
};
#endif