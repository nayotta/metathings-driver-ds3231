#ifndef _MT_MODULE_LORA_H
#define _MT_MODULE_LORA_H

#include "rs232_lora_ebyte.h"

// define =====================================================================

typedef struct _mt_module_lora_t {
  char *topic;
  uint8_t *buf;
  int size;
} mt_module_lora_t;

// help func ==================================================================

esp_err_t mt_module_lora_get_client_id(int32_t *id);

// func =======================================================================

void mt_module_lora_handle(char *topic, void *buf, int size);

#endif
