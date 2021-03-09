#ifndef MODBUS_GZPD800T_H
#define MODBUS_GZPD800T_H

#include "cJSON.h"

#include "esp_system.h"

#include "mt_mb.h"
#include "mt_mbport.h"

// define =====================================================================

typedef struct _gzpd800T_port_data_t {
  uint32_t amp;
  uint32_t freq;
  uint32_t power;
  bool warn;
} gzpd800T_port_data_t;

typedef struct _gzpd800T_data_t {
  uint32_t port_num;
  gzpd800T_port_data_t **port_data;
} gzpd800T_data_t;

// help func ==================================================================

gzpd800T_data_t *modbus_gzpd800T_new_data();

void modbus_gzpd800T_free_data(gzpd800T_data_t *data);

// global func ================================================================

esp_err_t modbus_gzpd800T_init(uint8_t port, int tx_pin, int rx_pin, int en_pin,
                               int port_num);

gzpd800T_data_t *modbus_gzpd800T_get_data();

cJSON *modbus_gzpd800T_get_flow_data();

#endif
