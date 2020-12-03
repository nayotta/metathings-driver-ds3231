#ifndef MODBUS_KTC_N01_H
#define MODBUS_KTC_N01_H

#include "mt_mb.h"
#include "mt_mbport.h"

#include "esp_err.h"

#include "mt_module_flow.h"

// define =====================================================================

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

// func =======================================================================

esp_err_t modbus_ktc_n01_get_temp_hum(int addr, double *temp, double *hum);

esp_err_t modbus_ktc_n01_get_current(int addr, double *current);

mt_module_flow_struct_group_t *modbus_ktc_n01_get_flow_data();

esp_err_t modbus_ktc_n01_set_cmd(int addr, int cmd);

esp_err_t modbus_ktc_n01_init(uint8_t port, int tx_pin, int rx_pin, int en_pin);

#endif
