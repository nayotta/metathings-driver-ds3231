#include "stdint.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "modbus_power001.h"

// global define ==============================================================
static const char *TAG = "modbus_power001_example";

int TX_PIN = 13;
int RX_PIN = 15;
int EN_PIN = 05;

int TEST_ADDR = 1;
int TEST_ADDR2 = 2;

// test func =================================================================
void test_power001_get_quality() {
  esp_err_t err = ESP_OK;
  double data = 0;

  err = modbus_power001_get_quality(TEST_ADDR, &data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  ESP_LOGI(TAG, "%4d %s ret:%f", __LINE__, __func__, data);

  return;
}

void test_power001_get_voltage() {
  esp_err_t err = ESP_OK;
  double data = 0;

  err = modbus_power001_get_voltage(TEST_ADDR, &data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  ESP_LOGI(TAG, "%4d %s ret:%f", __LINE__, __func__, data);

  return;
}

void test_power001_get_current() {
  esp_err_t err = ESP_OK;
  double data = 0;

  err = modbus_power001_get_current(TEST_ADDR, &data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  ESP_LOGI(TAG, "%4d %s ret:%f", __LINE__, __func__, data);

  return;
}

void test_power001_get_data() {
  modbus_power001_data_t *data = NULL;

  data = modbus_power001_get_data(TEST_ADDR2);
  if (data == NULL) {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    return;
  }

  ESP_LOGI(TAG, "%4d %s quality:%f voltage:%f current:%f", __LINE__, __func__,
           data->quality, data->voltage, data->current);
}

void test_power001_get_datas() {
  modbus_power001_datas_t *datas = NULL;

  datas = modbus_power001_get_datas();
  if (datas == NULL) {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    return;
  }

  for (int i = 0; i < datas->port_num; i++) {
    if (datas->datas[i] != NULL) {
      ESP_LOGI(TAG, "%4d %s index:%d quality:%f voltage:%f current:%f",
               __LINE__, __func__, i, datas->datas[i]->quality,
               datas->datas[i]->voltage, datas->datas[i]->current);
    } else {
      ESP_LOGI(TAG, "%4d %s index:%d NULL", __LINE__, __func__, i);
    }
  }
}

// main func ==================================================================
void app_main() {
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "test begin");

  err = modbus_power001_init(TX_PIN, RX_PIN, EN_PIN);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_init failed", __LINE__, __func__);
    return;
  }

  while (1) {
    // test get quality
    vTaskDelay(2000 / portTICK_RATE_MS);
    test_power001_get_quality();

    // test get voltage
    vTaskDelay(2000 / portTICK_RATE_MS);
    test_power001_get_voltage();

    // test get current
    vTaskDelay(2000 / portTICK_RATE_MS);
    test_power001_get_current(true);

    // test get data
    vTaskDelay(2000 / portTICK_RATE_MS);
    test_power001_get_data();

    // test get datas
    vTaskDelay(2000 / portTICK_RATE_MS);
    test_power001_get_datas();
    ESP_LOGI(TAG, "test end");
  }
}
