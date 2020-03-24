#ifndef _RS232_LORA_EBYTE_MODULE_MANAGE_H
#define _RS232_LORA_EBYTE_MODULE_MANAGE_H

#include "esp_err.h"

typedef struct _rs232_lora_ebyte_module_t {
  int module_index;
  int16_t id;
} rs232_lora_ebyte_module_t;

typedef struct _rs232_lora_ebyte_module_manage_t {
  int module_size;
  rs232_lora_ebyte_module_t **modules;
} rs232_lora_ebyte_module_manage_t;

esp_err_t rs232_lora_ebyte_module_manage_add(rs232_lora_ebyte_module_t *module);

esp_err_t
rs232_lora_ebyte_module_manage_get_id_by_module_index(int module_index,
                                                      int16_t *id_out);

esp_err_t rs232_lora_ebyte_module_manage_init();

#endif
