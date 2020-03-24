#ifndef _MODBUS_POWER001_SOTRAGE_H
#define _MODBUS_POWER001_SOTRAGE_H

#include "esp_err.h"
#include "esp_log.h"

// define =====================================================================

typedef struct _modbus_power001_config_t {
  int power_num;
  int *power_addr;
} modbus_power001_config_t;

// help func ==================================================================

modbus_power001_config_t *modbus_power001_storage_new_config(int size);

void modbus_power001_storage_free_config(modbus_power001_config_t *config);

esp_err_t
modbus_power001_storage_check_config(modbus_power001_config_t *config);

// func =======================================================================

modbus_power001_config_t *modbus_power001_storage_get_config();

esp_err_t modbus_power001_storage_set_config(modbus_power001_config_t *storage);

esp_err_t modbus_power001_storage_get_interval(int *interval);

esp_err_t modbus_power001_storage_set_interval(int interval);

#endif
