#include "sd.h"

sdMgr* sdMgr::mpInstance = NULL;

sdMgr::sdMgr()
{
    mpSdCfg = NULL;
    mpSomeipAgent = NULL;
    mpDatabase = NULL;
}

sdMgr::~sdMgr()
{
    if (mpSdCfg) {
        free(mpSdCfg);
        mpSdCfg = NULL;
    }
}

sdMgr* sdMgr::getInstance(void)
{
	if(mpInstance == NULL) {
		mpInstance = new sdMgr();
	}
	return mpInstance;
}

u32_t sdMgr::setSdConfig(sd_cfg_t *cfg)
{
    if (mpSdCfg == NULL) {
        //mpSdCfg = malloc()
    }

    return 0;
}

u32_t sdMgr::startSdService(void)
{
    mpSomeipAgent = new someipAgent;
    mpDatabase = new sdDatabase;

    return 0;
}

u32_t sdMgr::stopSdService(void)
{
    return 0;
}