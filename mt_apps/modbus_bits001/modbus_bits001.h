#ifndef MODBUS_BITS001_H
#define MODBUS_BITS001_H

#include "mt_mb.h"
#include "mt_mbport.h"

#include "esp_err.h"

// define =====================================================================

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

// func =======================================================================

esp_err_t modbus_bits001_init(uint8_t port, int tx_pin, int rx_pin, int en_pin);

esp_err_t modbus_bits001_get_temp(int addr, double *temp);

esp_err_t modbus_bits001_get_hum(int addr, double *hum);

esp_err_t modbus_bits001_get_frog(int addr, double *frog);

esp_err_t modbus_bits001_get_data(int addr, double *temp, double *hum,
                                  double *frog);

#endif
