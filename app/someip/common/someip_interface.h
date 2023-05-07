#ifndef __SOMEIP_INTERFACE_H__
#define __SOMEIP_INTERFACE_H__
#include <list>
#include "service_interface.h"

using namespace std;

typedef struct someip_service_cfg {
    u32_t role;
    service_key_t s_key;
    esomeip_endpoint_t  tcp;
    esomeip_endpoint_t  udp;
    esomeip_endpoint_t  mul_udp;
} someip_service_cfg_t;

typedef void (*message_handler_t)(u16_t service_id, u16_t instance_id, void *args, u8_t is_available);
typedef struct someip_method_s {
    u16_t method_id;
    void *args;
    message_handler_t  on_message;
} someip_method_t;

typedef struct someip_event_s {
    u16_t event_id;
    // u16_t group_id[16];         //属于某个事件组，1个event_id至少属于1个事件组，最多属于16个事件组 [PRS_SOMEIP_00366]
    void *args;
    message_handler_t  on_message;
} someip_event_t;

typedef struct someip_field_s {
    u16_t field_id;
    // u16_t group_id[16];         //属于某个事件组
    void *args;
    message_handler_t  get_func;   //query the value
    message_handler_t  set_func;    //change the value
} someip_field_t;

typedef struct someip_service_entry {
    someip_service_cfg_t cfg;
    list <someip_method_t *> mMethodList;  /**< 记录方法 */
    list <someip_event_t *> mEventList;  /**< 记录事件 */
    list <someip_field_t *> mFieldList;  /**< 记录field */
    /* client */
    u32_t is_available;

    /* server */
    available_handler_t on_available_handler;   /**< subscriber收到provider的offer消息后通知上层服务可用性的回调函数 */
    void *args;                        /**< 回调函数的参数 */
} someip_service_entry_t;



#endif