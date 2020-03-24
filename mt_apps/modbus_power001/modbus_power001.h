#ifndef _MODBUS_POWER001_H
#define _MODBUS_POWER001_H

#include "esp_err.h"
#include "esp_log.h"

#include "modbus_power001_storage.h"

// define =====================================================================

typedef struct _modbus_power001_data_t {
  double quality;
  double voltage;
  double current;
} modbus_power001_data_t;

typedef struct _modbus_power001_datas_t {
  int port_num;
  modbus_power001_data_t **datas;
} modbus_power001_datas_t;

// help func ==================================================================

modbus_power001_data_t *modbus_power001_new_data();

void modbus_power001_free_data(modbus_power001_data_t *data);

modbus_power001_datas_t *modbus_power001_new_datas(int size);

void modbus_power001_free_datas(modbus_power001_datas_t *datas);

// func =======================================================================

esp_err_t modbus_power001_init(uint8_t port,int tx_pin, int rx_pin, int en_pin);

// single api

esp_err_t modbus_power001_get_quality(uint8_t port, double *data);

esp_err_t modbus_power001_get_voltage(uint8_t port, double *data);

esp_err_t modbus_power001_get_current(uint8_t port, double *data);

// complex api

modbus_power001_data_t *modbus_power001_get_data(int port);

modbus_power001_datas_t *
modbus_power001_get_datas(modbus_power001_config_t *config);

#endif
