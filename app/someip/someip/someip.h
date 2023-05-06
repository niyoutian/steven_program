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
    u32_t createSomeipService(someip_service_cfg_t *service);
	// // u32_t deleteSomeipService(void);
    // // u32_t addSomeipServiceMethod(void);
	// // u32_t delSomeipServiceMethod(void);
	// u32_t sendRequest(u16_t serviceId, u16_t instanceId, u32_t methodId, u8_t *data, u32_t len);
	// u32_t startService(void);
	// u32_t stopService(void);

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