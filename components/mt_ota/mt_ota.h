#ifndef _MT_OTA_H
#define _MT_OTA_H

#include "esp_err.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

typedef struct _mt_ota_t {
  char *url;
} mt_ota_t;

mt_ota_t *mt_ota_default_new(char *url);

esp_err_t mt_ota_task(mt_ota_t *ota);

#endif
