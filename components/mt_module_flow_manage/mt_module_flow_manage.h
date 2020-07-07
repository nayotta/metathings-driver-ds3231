#ifndef _MT_MODULE_FLOW_MANAGE_H
#define _MT_MODULE_FLOW_MANAGE_H

#include "esp_err.h"
#include "stdint.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "mt_module_flow.h"

// define =====================================================================

typedef struct _mt_module_flow_manage_t {
  int flows_size;
  mt_module_flow_t **flows;
  QueueHandle_t *flows_handle;
} mt_module_flow_manage_t;

// func =======================================================================

esp_err_t mt_module_flow_manage_mqtt_process(char *topic, uint8_t *buf,
                                             int size);

esp_err_t mt_module_flow_manage_add(mt_module_flow_t *flow);

esp_err_t mt_module_flow_manage_get_index_by_module_id(char *module_id,
                                                       int *module_index);

mt_module_flow_t *mt_module_flow_manage_get_flow_by_flow_index(int flow_index);

#endif
