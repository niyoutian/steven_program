#ifndef __SOMEIP_H__
#define __SOMEIP_H__
#include <string>
#include "mxDef.h"
#include "service_interface.h"
#include "serviceDatabase.h"

using namespace std;

class someipMgr
{
public:
    static someipMgr* getInstance(void);

	u32_t initService(u32_t appid, string name);
	u32_t startService(void);
	u32_t stopService(void);

    u32_t createSomeipService(someip_service_cfg_t *service);
	u32_t enableSomeipService(u16_t service_id, u16_t instance_id, u32_t flag);
	u32_t deleteSomeipService(u16_t service_id, u16_t instance_id);

    u32_t addSomeipServiceMethod(u16_t service_id, u16_t instance_id, someip_method_t *method);
	u32_t addSomeipServiceEvent(u16_t service_id, u16_t instance_id, someip_event_t *event);
	u32_t addSomeipServiceField(u16_t service_id, u16_t instance_id, someip_field_t *field);

private:
	someipMgr();
	~someipMgr();

	static someipMgr* mpInstance;
	u32_t             mAppId;
	string            mAppName;
	u32_t             mStatus;

	serviceDatabase  *mpDatabase;      /**< 与someip service 数据库 */
};

#endif