#ifndef __SD_INTERFACE_H__
#define __SD_INTERFACE_H__

typedef struct findservice_entry_s
{
    u16_t service_id;
    u16_t instance_id;
} findservice_entry_t;

typedef struct findservice_entry_s
{
    u16_t service_id;
    u16_t instance_id;
} findservice_entry_t;


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

#endif
