#ifndef __SOMEIP_AGENT_H__
#define __SOMEIP_AGENT_H__
#include "mxDef.h"
#include <string>

using namespace std;


enum {
	SOMEIP_AGENT_API,
	SOMEIP_AGENT_DBUS,
	SOMEIP_AGENT_SOCKET 
};

typedef struct agent_dbus_cfg {
    string req_dbus_name;    /**< SOME/IP 集成 SOME/IP-SD的方式：api, dbus, socket  */
} agent_dbus_cfg_t;  


/**
* @class someipAgent
* @brief 负责与SOMEIP交互信息，可以是API，DBUS，socket 方式
* @author <list of authors>
* @note
* detailed description
*/
class someipAgent
{
public:
	someipAgent();
	~someipAgent();
    u32_t addOfferService(struct offerservice_entry *service);
private:
	u32_t mAgentMethod;    /**< 与SOMEIP交互信息方式，可以是api, dbus, socket  */
	u32_t mcheckTime;      /**< 交互信息方式为 dbus或者socket时，监测对端存活的时间 */

};
#endif
