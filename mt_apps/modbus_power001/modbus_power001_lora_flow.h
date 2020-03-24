#ifndef _MODBUS_POWER001_LORA_FLOW_H
#define _MODBUS_POWER001_LORA_FLOW_H

#include "esp_err.h"
#include "esp_log.h"
#include "string.h"

#include "modbus_power001_storage.h"

// define =====================================================================

// help func ==================================================================

esp_err_t modbus_power001_update_interval(int interval);

esp_err_t modbus_power001_updata_config(modbus_power001_config_t *config);

// func =======================================================================

esp_err_t modbus_power001_lora_flow_task();

#endif
