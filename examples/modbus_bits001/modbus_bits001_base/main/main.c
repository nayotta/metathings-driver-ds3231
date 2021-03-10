#include "stdint.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "modbus_bits001.h"

// global define ==============================================================

static const char *TAG = "modbus_bits001_base";

uint8_t RS485_PORT = 2;
int TX_PIN = 13;
int RX_PIN = 15;
int EN_PIN = 05;

// test func =================================================================

void test_bits001_get_temp_hum() {
  esp_err_t err = ESP_OK;
  double temp = 0;
  double hum = 0;

  err = modbus_bits001_get_temp_hum(1, &temp, &hum);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  return;
}

void test_bits001_get_frog() {
  esp_err_t err = ESP_OK;
  double frog = 0;

  err = modbus_bits001_get_frog(1, &frog);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  return;
}

// main func ==================================================================

void app_main() {
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "test begin");

  err = modbus_bits001_init(RS485_PORT, TX_PIN, RX_PIN, EN_PIN, 1,
                            MODBUS_BITS001_TYPE_TEMP_HUM_FOG);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_init failed", __LINE__, __func__);
    return;
  }

  while (1) {
    vTaskDelay(2000 / portTICK_RATE_MS);
    test_bits001_get_temp_hum();
    test_bits001_get_frog();
  }
}
