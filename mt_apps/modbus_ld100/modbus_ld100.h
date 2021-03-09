#ifndef MODBUS_LD100_H
#define MODBUS_LD100_H

#include "esp_err.h"
#include "esp_log.h"

#include "cJSON.h"

// global func ================================================================

esp_err_t modbus_ld100_get_addr(int addr, bool *state);

esp_err_t modbus_ld100_set_addr(int addr, int new_addr);

esp_err_t modbus_ld100_get_state(int addr, bool *state);

esp_err_t modbus_ld100_get_relay(int addr, bool *state);

esp_err_t modbus_ld100_set_relay(int addr, bool state);

esp_err_t modbus_ld100_init(uint8_t port, int tx_pin, int rx_pin, int en_pin);

esp_err_t modbus_ld100_get_has_changed(bool *change);

cJSON *modbus_ld100_get_flow_data();

#endif
