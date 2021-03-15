#include "stdint.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "cJSON.h"
#include "modbus_aew100.h"

// global define ==============================================================

static const char *TAG = "modbus_aew100_base";

UCHAR RS485_PORT = 2;
int TX_PIN = 13;
int RX_PIN = 15;
int EN_PIN = 05;

// test func =================================================================

void test_get_data() {
  cJSON *data = NULL;
  char *data_str = NULL;

  data = modbus_aew100_get_flow_data();
  if (data == NULL) {
    ESP_LOGE(TAG, "%4d %s modbus_aew100_get_flow_data failed", __LINE__,
             __func__);
    goto EXIT;
  }

  data_str = cJSON_Print(data);
  ESP_LOGI(TAG, "%4d %s data:%s", __LINE__, __func__, data_str);

EXIT:
  cJSON_Delete(data);
  return;
}

// main func ==================================================================

void app_main() {
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "test begin");

  err = modbus_aew100_init(RS485_PORT, TX_PIN, RX_PIN, EN_PIN, 1);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d modbus_aew100_init failed", __LINE__);
    return;
  }

  while (1) {
    // test get data
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    test_get_data();
  }
}