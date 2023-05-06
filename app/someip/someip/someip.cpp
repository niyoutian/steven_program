#include "someip.h"

someipMgr* someipMgr::mpInstance = NULL;

someipMgr::someipMgr()
{
	mAppId = 0;
	mpDatabase = new serviceDatabase;
}

someipMgr::~someipMgr()
{

}

someipMgr* someipMgr::getInstance(void)
{
	if(mpInstance == NULL) {
		mpInstance = new someipMgr();
	}
	return mpInstance;
}

u32_t someipMgr::initService(u32_t appid, string name)
{
	mAppId = appid;
	mAppName = name;
	return 0;
}

// u32_t someipMgr::startService(void)
// {

// 	while(mStatus) {
// 		sleep(1);
// 	}
// 	return 0;
// }

// u32_t someipMgr::stopService(void)
// {
// 	return 0;
// }

u32_t someipMgr::createSomeipService(someip_service_cfg_t *service)
{
	mpDatabase->addEntry(service);
	return 0;
}