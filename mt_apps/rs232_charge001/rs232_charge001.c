#include "rs232_charge001.h"

#include "rs232_dev.h"

// static define ==============================================================

static const char *TAG = "RS232_CHARGE001";

// help func ==================================================================

// global func ================================================================

esp_err_t rs232_charge001_init(int32_t uart_num, int32_t rx_pin,
                               int32_t tx_pin) {
  esp_err_t err = ESP_OK;
  rs232_dev_config_t *dev_config = rs232_dev_default_new();

  dev_config->uart_num = uart_num;
  dev_config->rx_pin = rx_pin;
  dev_config->tx_pin = tx_pin;
  dev_config->uart_config->baud_rate = 9600;
  dev_config->uart_config->data_bits = UART_STOP_BITS_2;

  err = rs232_dev_init(dev_config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_dev_init failed", __LINE__, __func__);
    return err;
  }

  err = rs232_charge001_sent_manage_init(dev_config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_sent_manage_init failed", __LINE__,
             __func__);
    return err;
  }

  err = rs232_charge001_recv_manage_init(dev_config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_recv_manage_init failed", __LINE__,
             __func__);
    return err;
  }

  ESP_LOGI(TAG, "%4d %s init success", __LINE__, __func__);

  return ESP_OK;
}

rs232_charge001_state2_t *rs232_charge001_get_state(int32_t port);

rs232_charge001_states_t *rs232_charge001_get_states();

esp_err_t rs232_charge001_set_charge();
