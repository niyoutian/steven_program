#ifndef __SD_H__
#define __SD_H__
#include "mxDef.h"
#include "service_interface.h"
#include "someipAgent.h"
#include "sdDatabase.h"



class sdMgr
{
public:
    static sdMgr* getInstance(void);
    u32_t setSdConfig(sd_cfg_t *cfg);
    u32_t startSdService(void);
    u32_t stopSdService(void);
private:
	sdMgr();
	~sdMgr();

	static sdMgr* mpInstance;

    sd_cfg_t    *mpSdCfg;       /**< some/ip-sd配置信息 */
    someipAgent *mpSomeipAgent;   /**< 与some/ip通信的代理 */
    sdDatabase  *mpDatabase;      /**< 与some/ipsd 数据库 */

    // list<*> mRemoteServiceList;  /**< 记录对端提供的Service */
    // sd state machine  
};

#endif