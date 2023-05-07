#ifndef __SERVICE_INTERFACE_H__
#define __SERVICE_INTERFACE_H__
#include "mxDef.h"


enum {
	SOMEIP_ROLE_PROVIDER,
	SOMEIP_ROLE_SUBSCRIBER
};

typedef struct service_key_s
{
    u16_t service_id;
    u16_t instance_id;
} service_key_t;





struct endpoint
{
  u32_t v4_addr;
  u16_t v4_port;
};

 /**
 * @brief Offer Service Entry 结构体
 * @note Configuration/Load Balancing option暂时不支持
 */
struct offerservice_entry
{
    u16_t service_id;
    u16_t instance_id;
    u8_t major_version;
    u8_t reserve[3];
    u32_t minor_version;
    u32_t lifetime;
    struct endpoint *v4udp_endp;
    struct endpoint *v4multi_endp;
    struct endpoint *v4tcp_endp;
    struct endpoint *v6udp_endp;
    struct endpoint *v6multi_endp;
    struct endpoint *v6tcp_endp;
};

struct findservice_entry
{
    u16_t service_id;
    u16_t instance_id;
    u8_t major_version;
    u8_t reserve[3];
    u32_t minor_version;
};



/*************************************************************************************************************/
// 上层APP向SOMEIP模块配置服务实例
typedef struct esomeip_endpoint {
  u32_t v4_addr;
  u16_t v4_port;
} esomeip_endpoint_t ;

typedef void (*available_handler_t)(u16_t service_id, u16_t instance_id, void *args, u8_t is_available);

typedef struct someip_instance_cfg {
    u8_t role;                 /**< subscriber or provider */
    u16_t service_id;
    u16_t instance_id;
    esomeip_endpoint_t  *tcp;
    esomeip_endpoint_t  *udp;
    esomeip_endpoint_t  *mul_udp;
    available_handler_t on_available_handler;   /**< subscriber收到provider的offer消息后通知上层服务可用性的回调函数 */
    void *args;                        /**< 回调函数的参数 */
} someip_instance_cfg_t;


#endif
