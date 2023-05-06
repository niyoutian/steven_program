#include "someipAgent.h"

someipAgent::someipAgent()
{
}

someipAgent::~someipAgent()
{
}

 /**
 * @brief		add offer service
 * @details	    接收SOME/IP的消息进行相应的处理 
 * @param[in]	inArgName input argument description.
 * @param[out]	outArgName output argument description. 
 * @retval		OK		成功
 * @retval		ERROR	错误 
 * @par 标识符
 * 		保留
 * @par 其它
 * 		无
 * @par 修改日志
 * 		XXX于2020-07-06创建
 * @note 《AUTOSAR_PRS_SOMEIPServiceDiscoveryProtocol.pdf》 5.1.2.5 Service Entries
 */
u32_t someipAgent::addOfferService(struct offerservice_entry *service)
{

}
