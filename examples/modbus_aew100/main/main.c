#include "stdint.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "modbus_aew100.h"

// global define ==============================================================
static const char *TAG = "modbus_aew100_example";

UCHAR RS485_PORT = 2;
ULONG RS485_BAUD = 9600;
eMBParity RS485_PARITY = MB_PAR_NONE;
int TX_PIN = 13;
int RX_PIN = 15;
int EN_PIN = 05;

UCHAR ADDR = 27;

// test func =================================================================
void test_aew100_get_data() {
  esp_err_t err = ESP_OK;
  bool state = true;
  Aew100_data_t data;

  err = mt_aew100_get_data(ADDR, &data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  ESP_LOGI(TAG, "%4d %s currentA:%.2f ", __LINE__, __func__, data.currentA);
  ESP_LOGI(TAG, "%4d %s currentB:%.2f ", __LINE__, __func__, data.currentB);
  ESP_LOGI(TAG, "%4d %s currentC:%.2f ", __LINE__, __func__, data.currentC);
  ESP_LOGI(TAG, "%4d %s votageA:%.2f ", __LINE__, __func__, data.votageA);
  ESP_LOGI(TAG, "%4d %s votageB:%.2f ", __LINE__, __func__, data.votageB);
  ESP_LOGI(TAG, "%4d %s votageC:%.2f ", __LINE__, __func__, data.votageC);
  ESP_LOGI(TAG, "%4d %s activePowerA:%.2f ", __LINE__, __func__,
           data.activePowerA);
  ESP_LOGI(TAG, "%4d %s activePowerB:%.2f ", __LINE__, __func__,
           data.activePowerB);
  ESP_LOGI(TAG, "%4d %s activePowerC:%.2f ", __LINE__, __func__,
           data.activePowerC);
  ESP_LOGI(TAG, "%4d %s reactivePowerA:%.2f ", __LINE__, __func__,
           data.reactivePowerA);
  ESP_LOGI(TAG, "%4d %s reactivePowerB:%.2f ", __LINE__, __func__,
           data.reactivePowerB);
  ESP_LOGI(TAG, "%4d %s reactivePowerC:%.2f ", __LINE__, __func__,
           data.reactivePowerC);
  ESP_LOGI(TAG, "%4d %s powerFactorA:%.2f ", __LINE__, __func__,
           data.powerFactorA);
  ESP_LOGI(TAG, "%4d %s powerFactorB:%.2f ", __LINE__, __func__,
           data.powerFactorB);
  ESP_LOGI(TAG, "%4d %s powerFactorC:%.2f ", __LINE__, __func__,
           data.powerFactorC);
  ESP_LOGI(TAG, "%4d %s qualityA:%.2f ", __LINE__, __func__, data.qualityA);
  ESP_LOGI(TAG, "%4d %s qualityB:%.2f ", __LINE__, __func__, data.qualityB);
  ESP_LOGI(TAG, "%4d %s qualityC:%.2f ", __LINE__, __func__, data.qualityC);
  return;
}

// main func ==================================================================
void app_main() {
  eMBErrorCode emb_ret = 0;

  ESP_LOGI(TAG, "test begin");

  emb_ret = modbus_aew100_init(RS485_PORT, RS485_BAUD, RS485_PARITY, TX_PIN,
                               RX_PIN, EN_PIN);
  if (emb_ret != 0) {
    ESP_LOGE(TAG, "%4d modbus_init failed", __LINE__);
    return;
  }

  mt_modbus_aew100_task();

  ESP_LOGI(TAG, "test end");

  while (1) {
    test_aew100_get_data();
    vTaskDelay(2000 / portTICK_RATE_MS);
  }
}
