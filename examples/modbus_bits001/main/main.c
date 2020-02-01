#include "stdint.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "modbus_bits001.h"

// global define ==============================================================
static const char *TAG = "modbus_bits001_example";

UCHAR RS485_PORT = 2;
ULONG RS485_BAUD = 9600;
eMBParity RS485_PARITY = MB_PAR_NONE;
int TX_PIN = 23;
int RX_PIN = 22;
int EN_PIN = 18;

UCHAR ADDR = 1;

// test func =================================================================
void test_bits001_get_temp() {
  esp_err_t err = ESP_OK;
  double temp = 0;

  err = mt_bits001_get_temp(ADDR, &temp);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  return;
}

void test_bits001_get_hum() {
  esp_err_t err = ESP_OK;
  double hum = 0;

  err = mt_bits001_get_hum(ADDR, &hum);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  return;
}

void test_bits001_get_frog() {
  esp_err_t err = ESP_OK;
  double frog = 0;

  err = mt_bits001_get_frog(ADDR, &frog);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  return;
}

// main func ==================================================================
void app_main() {
  eMBErrorCode emb_ret = 0;

  ESP_LOGI(TAG, "test begin");

  emb_ret = modbus_bits001_init(RS485_PORT, RS485_BAUD, RS485_PARITY, TX_PIN,
                                RX_PIN, EN_PIN);
  if (emb_ret != 0) {
    ESP_LOGE(TAG, "%4d modbus_init failed", __LINE__);
    return;
  }

  mt_modbus_bits001_task();

  ESP_LOGI(TAG, "test end");

  while (1) {
    vTaskDelay(2000 / portTICK_RATE_MS);
    test_bits001_get_temp();
    test_bits001_get_hum();
    test_bits001_get_frog();
  }
}
