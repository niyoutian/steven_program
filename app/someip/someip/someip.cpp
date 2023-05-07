#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

u32_t someipMgr::startService(void)
{
	mStatus = true;

	while(mStatus) {
		sleep(1);
	}
	return 0;
}

u32_t someipMgr::stopService(void)
{
	return 0;
}

u32_t someipMgr::createSomeipService(someip_service_cfg_t *service)
{
	u32_t ret = 0;

	ret = mpDatabase->addEntry(service);

	return ret;
}

u32_t someipMgr::enableSomeipService(u16_t service_id, u16_t instance_id, u32_t flag)
{
	return 0;
}

u32_t someipMgr::deleteSomeipService(u16_t service_id, u16_t instance_id)
{
	return 0;
}

u32_t someipMgr::addSomeipServiceMethod(u16_t service_id, u16_t instance_id, someip_method_t *method)
{
	u32_t ret = 0;

	ret = mpDatabase->addMethod(service_id, instance_id, method);

	return ret;
}

u32_t someipMgr::addSomeipServiceEvent(u16_t service_id, u16_t instance_id, someip_event_t *event)
{
	u32_t ret = 0;

	ret = mpDatabase->addEvent(service_id, instance_id, event);

	return ret;
}

u32_t someipMgr::addSomeipServiceField(u16_t service_id, u16_t instance_id, someip_field_t *field)
{
	u32_t ret = 0;

	ret = mpDatabase->addField(service_id, instance_id, field);

	return ret;
}