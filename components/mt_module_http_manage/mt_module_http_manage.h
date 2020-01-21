#ifndef _MT_MODULE_HTTP_MANAGE_H
#define _MT_MODULE_HTTP_MANAGE_H

#include "esp_err.h"
#include "stdint.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "mt_module_http.h"

typedef struct _mt_module_http_manage_t {
  int module_size;
  mt_module_http_t **moudle_https;
  int *module_index;
} mt_module_http_manage_t;

esp_err_t mt_module_http_manage_add(mt_module_http_t *module_http,
                                    int module_index);

esp_err_t mt_module_http_manage_get_index_by_module_id(char *module_id,
                                                       int *module_index);

#endif
