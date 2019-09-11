#ifndef RS232_dev_H
#define RS232_dev_H

#include "driver/uart.h"
#include "esp_log.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

typedef struct _rs232_dev_config_t
{
  int uart_num;
  int rx_pin;
  int tx_pin;
  int rts_pin;
  int cts_pin;
  uart_config_t *uart_config;
  int buf_max_size;
  int timeout;
} rs232_dev_config_t;

uint8_t *rs232_dev_read(rs232_dev_config_t *dev_config, int *buf_size_out);

esp_err_t rs232_dev_write(rs232_dev_config_t *dev_config, uint8_t *buf_in,
                          int buf_size_in);

rs232_dev_config_t *rs232_dev_default_new();

void rs232_dev_config_free(rs232_dev_config_t *dev_config);

esp_err_t rs232_dev_init(rs232_dev_config_t *dev_config);

#endif