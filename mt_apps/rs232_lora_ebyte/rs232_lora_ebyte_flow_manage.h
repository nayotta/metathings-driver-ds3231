#ifndef _RS232_LORA_EBYTE_FLOW_MANAGE_H
#define _RS232_LORA_EBYTE_FLOW_MANAGE_H

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "mt_module_flow.h"

typedef struct _rs232_lora_ebyte_flow_manage_t {
  int flows_size;
  mt_module_flow_t **flows;
  int *flows_addr;
  QueueHandle_t **flows_handle;
} rs232_lora_ebyte_flow_manage_t;

esp_err_t rs232_lora_ebyte_flow_manage_add(mt_module_flow_t *flow,
                                           int flow_addr,
                                           QueueHandle_t flow_handle);

rs232_lora_ebyte_flow_manage_t *rs232_lora_ebyte_flow_manage_get();

#endif
