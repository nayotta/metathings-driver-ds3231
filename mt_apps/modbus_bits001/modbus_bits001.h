#ifndef MODBUS_BITS001_H
#define MODBUS_BITS001_H

#include "cJSON.h"

#include "esp_err.h"

// define =====================================================================

enum MODBUS_BITS001_TYPE {
  MODBUS_BITS001_TYPE_TEMP_HUM = 1,
  MODBUS_BITS001_TYPE_FOG = 2,
  MODBUS_BITS001_TYPE_TEMP_HUM_FOG = 3
};

// global func ================================================================

esp_err_t modbus_bits001_init(uint8_t port, int tx_pin, int rx_pin, int en_pin,
                              int device_num, enum MODBUS_BITS001_TYPE type);

esp_err_t modbus_bits001_with_config_init(uint8_t port, int tx_pin, int rx_pin,
                                          int en_pin);

esp_err_t modbus_bits001_get_temp_hum(int addr, double *temp, double *hum);

esp_err_t modbus_bits001_get_frog(int addr, double *frog);

esp_err_t modbus_bits001_get_data(int addr, double *temp, double *hum,
                                  double *frog);

// json api func ==============================================================

cJSON *modbus_bits001_get_flow_data();

#endif
