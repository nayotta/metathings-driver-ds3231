#ifndef _MODBUS_AIRSIWTCH001_UTILS_H
#define _MODBUS_AIRSIWTCH001_UTILS_H

#include "esp_err.h"
#include "modbus_airswitch001.h"
#include "mt_proto_airswitch001.pb-c.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

esp_err_t modbus_airswitch001_util_copy_datas(airswitch_data_t *datas_in,
                                              Data *datas_out);

esp_err_t modbus_airswitch001_util_copy_configs(airswitch_config_t *config_in,
                                                Config *config_out);

esp_err_t modbus_airswitch001_util_check_get_req(int32_t *addr, GetReq *req);

esp_err_t modbus_airswitch001_util_check_set_state_req(int32_t *addr,
                                                       SetStateReq *req);

esp_err_t modbus_airswitch001_util_check_set_config_req(int32_t *addr,
                                                        SetConfigReq *req);

esp_err_t modbus_airswitch001_util_check_set_quality_req(int32_t *addr,
                                                         SetQualityReq *req);

esp_err_t modbus_airswitch001_util_check_set_leak_test_req(int32_t *addr,
                                                           SetLeakTestReq *req);

#endif