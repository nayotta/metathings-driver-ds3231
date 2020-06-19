#ifndef _MODBUS_RECLOSE001_H
#define _MODBUS_RECLOSE001_H

#include "esp_err.h"
#include "esp_log.h"

// define =====================================================================

typedef struct _modbus_reclose001_data_t {
  bool state;
  bool if_change_state;
  bool warn_high_voltage;
  bool if_change_warn_high_voltage;
  bool warn_low_voltage;
  bool if_change_warn_low_voltage;
  bool warn_overload;
  bool if_change_warn_overload;
  bool warn_short;
  bool if_change_warn_short;
  bool warn_leak;
  bool if_change_warn_leak;
} modbus_reclose001_data_t;

// help func ==================================================================

modbus_reclose001_data_t *modbus_reclose001_new_data();

void modbus_reclose001_free_data(modbus_reclose001_data_t *data);

// func =======================================================================

esp_err_t modbus_reclose001_task(uint8_t port, int tx_pin, int rx_pin,
                                 int en_pin);

esp_err_t modbus_reclose001_get_data(uint8_t port,
                                     modbus_reclose001_data_t *data);

esp_err_t modbus_reclose001_get_data_and_check_if_change(
    uint8_t port, modbus_reclose001_data_t *data,
    modbus_reclose001_data_t *data_save, bool *change);

#endif
