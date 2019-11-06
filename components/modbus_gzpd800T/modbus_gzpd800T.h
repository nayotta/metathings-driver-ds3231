#ifndef MODBUS_LD100_H
#define MODBUS_LD100_H

#include "mt_mb.h"
#include "mt_mbport.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

typedef struct _gzpd800T_4ch_data_t
{
  uint32_t amp1;
  uint32_t freq1;
  uint32_t power1;
  uint32_t amp2;
  uint32_t freq2;
  uint32_t power2;
  uint32_t amp3;
  uint32_t freq3;
  uint32_t power3;
  uint32_t amp4;
  uint32_t freq4;
  uint32_t power4;
} gzpd800T_4ch_data_t;

esp_err_t mt_gzpd800T_get_4ch_data(gzpd800T_4ch_data_t *data);

esp_err_t mt_gzpd800T_get_warn(int addr, bool *warn);

#endif