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

// define =====================================================================

typedef struct _gzpd800T_4ch_data_t {
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

typedef struct _gzpd800T_8ch_data_t {
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
  uint32_t amp5;
  uint32_t freq5;
  uint32_t power5;
  uint32_t amp6;
  uint32_t freq6;
  uint32_t power6;
  uint32_t amp7;
  uint32_t freq7;
  uint32_t power7;
  uint32_t amp8;
  uint32_t freq8;
  uint32_t power8;
} gzpd800T_8ch_data_t;

// func =======================================================================

esp_err_t modbus_gzpd800T_get_4ch_data(gzpd800T_4ch_data_t *data);

esp_err_t modbus_gzpd800T_get_8ch_data(gzpd800T_8ch_data_t *data);

esp_err_t modbus_gzpd800T_get_warn(int addr, bool *warn);

esp_err_t modbus_gzpd800T_init(uint8_t port, int tx_pin, int rx_pin,
                               int en_pin);

#endif
