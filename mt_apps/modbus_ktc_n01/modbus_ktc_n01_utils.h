#ifndef _MODBUS_KTC_N01_UTILS_H
#define _MODBUS_KTC_N01_UTILS_H

#include "esp_err.h"
#include "modbus_ktc_n01.h"
#include "mt_proto_ktc_n01.pb-c.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

esp_err_t modbus_ktc_n01_util_check_set_cmd_req(MtKtcN01__SetCmdReq *req);

#endif
