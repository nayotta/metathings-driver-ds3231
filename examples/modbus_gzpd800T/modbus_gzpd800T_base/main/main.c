#include "stdint.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "modbus_gzpd800T.h"

// global define ==============================================================

static const char *TAG = "modbus_gzpd800T_base";

#define GZPD800T_PORT_NUM 4

UCHAR RS485_PORT = 2;
int TX_PIN = 13;
int RX_PIN = 15;
int EN_PIN = 05;

// test func =================================================================
void test_get_data() {
  esp_err_t err = ESP_OK;
  gzpd800T_data_t *data = NULL;

  data = modbus_gzpd800T_get_data();
  if (data == NULL) {
    ESP_LOGE(TAG, "%4d %s modbus_gzpd800T_get_data failed", __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

EXIT:
  modbus_gzpd800T_free_data(data);
  return;
}

// main func ==================================================================
void app_main() {
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "test begin");

  err = modbus_gzpd800T_init(RS485_PORT, TX_PIN, RX_PIN, EN_PIN,
                             GZPD800T_PORT_NUM);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d modbus_init failed", __LINE__);
    return;
  }

  while (1) {
    // test get data
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    test_get_data();
  }
