#ifndef MODBUS_LD100_H
#define MODBUS_LD100_H

#include "esp_err.h"
#include "esp_log.h"

#include "mt_module_flow.h"

esp_err_t modbus_ld100_get_addr(int addr, bool *state);

esp_err_t modbus_ld100_set_addr(int addr, int new_addr);

esp_err_t modbus_ld100_get_state(int addr, bool *state);

esp_err_t modbus_ld100_get_relay(int addr, bool *state);

esp_err_t modbus_ld100_set_relay(int addr, bool state);

esp_err_t modbus_ld100_init(uint8_t port, int tx_pin, int rx_pin, int en_pin);

mt_module_flow_struct_group_t *modbus_ld100_get_data(uint8_t port);

#endif
