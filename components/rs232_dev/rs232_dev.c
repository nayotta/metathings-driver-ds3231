#include "stdio.h"
#include "string.h"

#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"

#include "rs232_dev.h"

// global define ==============================================================

static const char *TAG = "RS232_dev";

// global func ================================================================

/*uint8_t *rs232_dev_read_debug(rs232_dev_config_t *dev_config,
                              int *buf_size_out) {
  return NULL;
}

esp_err_t rs232_dev_write(rs232_dev_config_t *dev_config, uint8_t *buf_in,
                          int buf_size_in) {
  return ESP_OK;
};

rs232_dev_config_t *rs232_dev_default_new() { return NULL; };

 void rs232_dev_config_free(rs232_dev_config_t *dev_config) { return; };

esp_err_t rs232_dev_init(rs232_dev_config_t *dev_config) { return ESP_OK; };
*/

uint8_t *rs232_dev_read(rs232_dev_config_t *dev_config, int *buf_size_out) {
  // return NULL; // debug

  uint8_t *buf_temp = malloc(dev_config->buf_max_size * sizeof(uint8_t));
  uint8_t *buf_out = NULL;

  *buf_size_out =
      uart_read_bytes(dev_config->uart_num, buf_temp, dev_config->buf_max_size,
                      dev_config->timeout / portTICK_RATE_MS);

  if (*buf_size_out > 0) {
    buf_out = malloc(*buf_size_out * sizeof(uint8_t));
    memcpy(buf_out, buf_temp, *buf_size_out);
  }

  free(buf_temp);
  return buf_out;
}

esp_err_t rs232_dev_write(rs232_dev_config_t *dev_config, uint8_t *buf_in,
                          int buf_size_in) {
  esp_err_t err = ESP_OK;
  int ret = 0;

  ret = uart_write_bytes(dev_config->uart_num, (char *)buf_in, buf_size_in);
  if (ret != buf_size_in) {
    ESP_LOGE(TAG, "%4d %s uart_write_bytes send %d != need sent %d", __LINE__,
             __func__, ret, buf_size_in);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return err;
}

rs232_dev_config_t *rs232_dev_default_new() {
  rs232_dev_config_t *config_out = malloc(sizeof(rs232_dev_config_t));
  config_out->uart_config = malloc(sizeof(uart_config_t));

  config_out->uart_num = UART_NUM_1;
  config_out->tx_pin = 16;
  config_out->rx_pin = 04;
  config_out->rts_pin = UART_PIN_NO_CHANGE;
  config_out->cts_pin = UART_PIN_NO_CHANGE;
  config_out->buf_max_size = 1024;
  config_out->timeout = 20;

  config_out->uart_config->baud_rate = 9600;
  config_out->uart_config->data_bits = UART_DATA_8_BITS;
  config_out->uart_config->parity = UART_PARITY_DISABLE;
  config_out->uart_config->stop_bits = UART_STOP_BITS_1;
  config_out->uart_config->flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
  config_out->uart_config->rx_flow_ctrl_thresh = 0;

  return config_out;
}

void rs232_dev_config_free(rs232_dev_config_t *dev_config) {
  if (dev_config->uart_config != NULL)
    free(dev_config->uart_config);

  if (dev_config != NULL) {
    free(dev_config);
    dev_config = NULL;
  }
}

esp_err_t rs232_dev_init(rs232_dev_config_t *dev_config) {
  esp_err_t err = ESP_OK;

  err = uart_param_config(dev_config->uart_num, dev_config->uart_config);
  ESP_LOGI(TAG,
           "%4d %s uart_set_pin, uart_num:%d, tx_pin:%d, rx_pin:%d, "
           "rts:%d, cts:%d, "
           "band:%d, bit:%d, parrity:%d, stop:%d, flowctrl:%d, err=%d",
           __LINE__, __func__, dev_config->uart_num, dev_config->tx_pin,
           dev_config->rx_pin, dev_config->rts_pin, dev_config->cts_pin,
           dev_config->uart_config->baud_rate,
           dev_config->uart_config->data_bits, dev_config->uart_config->parity,
           dev_config->uart_config->stop_bits,
           dev_config->uart_config->flow_ctrl, err);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s uart_set_pin failed", __LINE__, __func__);
    return err;
  }

  err =
      uart_set_pin(dev_config->uart_num, dev_config->tx_pin, dev_config->rx_pin,
                   dev_config->rts_pin, dev_config->cts_pin);
  if (err != ESP_OK) {
    ESP_LOGE(TAG,
             "%4d %s uart_set_pin failed, uart_num:%d, tx_pin:%d, rx_pin:%d, "
             "rts:%d, cts:%d, err=%d",
             __LINE__, __func__, dev_config->uart_num, dev_config->tx_pin,
             dev_config->rx_pin, dev_config->rts_pin, dev_config->cts_pin, err);
    return err;
  }

  err = uart_driver_install(dev_config->uart_num, dev_config->buf_max_size * 2,
                            0, 0, NULL, 0);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s uart_driver_install failed, err=%d", __LINE__,
             __func__, err);
    return err;
  }

  return ESP_OK;
}
