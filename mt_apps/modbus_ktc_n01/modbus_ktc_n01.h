#ifndef _MODBUS_KTC_N01_H
#define _MODBUS_KTC_N01_H

#include "mt_mb.h"
#include "mt_mbport.h"

#include "esp_err.h"

#include "cJSON.h"

// func =======================================================================

esp_err_t modbus_ktc_n01_get_temp_hum(int addr, double *temp, double *hum);

esp_err_t modbus_ktc_n01_get_current(int addr, double *current);

esp_err_t modbus_ktc_n01_set_cmd(int addr, int cmd);

esp_err_t modbus_ktc_n01_init(uint8_t port, int tx_pin, int rx_pin, int en_pin,
                              int device_num);

esp_err_t modbus_ktc_n01_with_config_init(uint8_t port, int tx_pin, int rx_pin,
                                          int en_pin);

// json api func ==============================================================

cJSON *modbus_ktc_n01_get_flow_data();

cJSON *modbus_ktc_n01_json_set_cmd(cJSON *paras);

#endif
