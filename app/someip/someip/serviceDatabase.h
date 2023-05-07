#ifndef __SERVICE_DATABASE_H__
#define __SERVICE_DATABASE_H__
#include <list>
#include "mxDef.h"
#include "mxMutex.h"
#include "someip_interface.h"

using namespace std;

class serviceDatabase
{
public:
	serviceDatabase();
	~serviceDatabase();
    u32_t addEntry(someip_service_cfg_t *service);
    u32_t addMethod(u16_t service_id, u16_t instance_id, someip_method_t *method);
    u32_t addEvent(u16_t service_id, u16_t instance_id, someip_event_t *event);
    u32_t addField(u16_t service_id, u16_t instance_id, someip_field_t *field);
    // u32_t updateEntry(struct offerservice_entry *service);
private:
    mxMonitoredMutex	mServiceListMutex; 
    list <someip_service_entry_t *> mServiceList;  /**< 记录对端提供的Service */
};
#endif