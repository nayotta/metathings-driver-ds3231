#ifndef _MT_MODULE_LORA_H
#define _MT_MODULE_LORA_H

#include "rs232_lora_ebyte.h"

typedef struct _mt_module_lora_t {
  char *topic;
  uint8_t *buf;
  int size;
} mt_module_lora_t;

void mt_module_lora_handle(char *topic, void *buf, int size);

#endif
