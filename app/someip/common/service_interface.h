#ifndef __SERVICE_INTERFACE_H__
#define __SERVICE_INTERFACE_H__
#include "mxDef.h"


enum {
	SOMEIP_ROLE_PROVIDER,
	SOMEIP_ROLE_SUBSCRIBER
};

 /**
 * @brief sd_cfg 结构体
 * @note 《AUTOSAR_PRS_SOMEIPServiceDiscoveryProtocol.pdf》 6 Configuration Parameters
 */
typedef struct sd_cfg {
    u32_t integration_method;    /**< SOME/IP 集成 SOME/IP-SD的方式：api, dbus, socket  */
    u32_t initial_delay_min;    /**< PRS_SOMEIPSD_00399 */
    u32_t initial_delay_max;    /**< PRS_SOMEIPSD_00400 */
    u32_t repetitions_base_delay;    /**< PRS_SOMEIPSD_00405 */
    u32_t repetitions_max;          /**< PRS_SOMEIPSD_00407 */
    u32_t cyclic_offer_delay;          /**< PRS_SOMEIPSD_00411 */
    u32_t request_response_delay_minimum;     /**< PRS_SOMEIPSD_00417 */
    u32_t request_response_delay_maximum;     /**< PRS_SOMEIPSD_00420 */
} sd_cfg_t;  


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


/*************************************************************************************************************/
typedef void (*message_handler_t)(u16_t service_id, u16_t instance_id, void *args, u8_t is_available);
typedef struct someip_method {
    u16_t method_id;
    void *args;
    message_handler_t  on_message;
} someip_method_t;

typedef struct someip_event {
    u16_t event_id;
    u16_t group_id[16];         //属于某个事件组，1个event_id至少属于1个事件组，最多属于16个事件组 [PRS_SOMEIP_00366]
    void *args;
    message_handler_t  on_message;
} someip_event_t;

typedef struct someip_field {
    u16_t field_id;
    u16_t group_id[16];         //属于某个事件组
    void *args;
    message_handler_t  get_func;   //query the value
    message_handler_t  set_func;    //change the value
} someip_field_t;

typedef struct someip_service_method {
    u16_t service_id;
    u16_t instance_id;

} someip_service_method_t;


typedef struct someip_service_cfg {
    u32_t role;
    u16_t service_id;
    u16_t instance_id;
    esomeip_endpoint_t  tcp;
    esomeip_endpoint_t  udp;
    esomeip_endpoint_t  mul_udp;
} someip_service_cfg_t;


typedef struct someip_service_entry {
    u32_t role;
    u16_t service_id;
    u16_t instance_id;
    esomeip_endpoint_t  tcp;
    esomeip_endpoint_t  udp;
    esomeip_endpoint_t  mul_udp;
    available_handler_t on_available_handler;   /**< subscriber收到provider的offer消息后通知上层服务可用性的回调函数 */
    void *args;                        /**< 回调函数的参数 */
} someip_service_entry_t;

#endif
