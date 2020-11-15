#ifndef _MODBUS_RECLOSE001_H
#define _MODBUS_RECLOSE001_H

#include "esp_err.h"
#include "esp_log.h"

#include "mt_module_flow.h"

// func =======================================================================

esp_err_t modbus_reclose001_task(uint8_t port, int tx_pin, int rx_pin,
                                 int en_pin);

mt_module_flow_struct_group_t *modbus_reclose001_get_data(uint8_t port);

esp_err_t modbus_reclose001_diff(mt_module_flow_struct_group_t *data1,
                                 mt_module_flow_struct_group_t *data2,
                                 bool *change);

#endif
