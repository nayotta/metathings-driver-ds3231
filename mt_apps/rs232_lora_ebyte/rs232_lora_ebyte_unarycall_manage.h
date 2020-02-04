#ifndef _RS232_LORA_EBYTE_UNARYCALL_MANAGE_H
#define _RS232_LORA_EBYTE_UNARYCALL_MANAGE_H

#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef struct _rs232_lora_ebyte_unarycall_t {
  QueueHandle_t handle; // queue
  int64_t create_time;  // create time
} rs232_lora_ebyte_unarycall_t;

typedef struct _rs232_lora_ebyte_unarycall_manage_t {
  int unarycall_size;
  rs232_lora_ebyte_unarycall_t **unarycall_handle;
} rs232_lora_ebyte_unarycall_manage_t;

esp_err_t rs232_lora_ebyte_unarycall_manage_add(QueueHandle_t handle);

esp_err_t rs232_lora_ebyte_unarycall_manage_del(QueueHandle_t handle);

#endif
