#ifndef _MODBUS_AEW100_UTILS_H
#define _MODBUS_AEW100_UTILS_H

#include "esp_err.h"
#include "modbus_aew100.h"
#include "mt_proto_aew100.pb-c.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

esp_err_t modbus_aew100_util_check_get_data_req(int32_t *addr,
                                                MtAew100__GetDataReq *req);

esp_err_t
modbus_aew100_util_check_get_current_req(int32_t *addr,
                                         MtAew100__GetCurrentReq *req);

#endif
