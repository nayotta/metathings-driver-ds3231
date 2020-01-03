#ifndef _MT_MODULE_FLOW_MANAGE_H
#define _MT_MODULE_FLOW_MANAGE_H

#include "esp_err.h"
#include "stdint.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "mt_module_flow.h"

typedef struct _mt_module_flow_manage_t {
  int flows_size;
  mt_module_flow_t **flows;
  QueueHandle_t *flows_handle;
} mt_module_flow_manage_t;

esp_err_t mt_module_flow_manage_mqtt_process(char *topic, uint8_t *buf,
                                             int size);

esp_err_t mt_module_flow_manage_add(mt_module_flow_t *flow);

#endif
