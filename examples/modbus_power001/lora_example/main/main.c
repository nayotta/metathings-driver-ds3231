#include "stdint.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "modbus_power001.h"
#include "rs232_lora_ebyte.h"
#include "modbus_power001_lora_flow.h"

// global define ==============================================================
static const char *TAG = "modbus_power001_example_lora_test";

int RS485_PORT = 2;
int TX_PIN = 13;
int RX_PIN = 15;
int EN_PIN = 05;

int RS232_PORT = 1;
int LORA_TX = 10;
int LORA_RX = 9;
int LORA_BAUD = 9600;

// test func =================================================================

void test_power001_get_datas() {
  modbus_power001_datas_t *datas = NULL;
  modbus_power001_config_t *config = NULL;

  config = modbus_power001_storage_get_config();
  if (config == NULL) {
    ESP_LOGE(TAG, "%4d %s modbus_power001_storage_get_config failed", __LINE__,
             __func__);
    return;
  }

  datas = modbus_power001_get_datas(config);
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

  err = modbus_power001_init(RS485_PORT, TX_PIN, RX_PIN, EN_PIN);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_init failed", __LINE__, __func__);
    return;
  }

  err = rs232_lora_ebyte_init(RS232_PORT, LORA_RX, LORA_TX, LORA_BAUD);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_init failed", __LINE__, __func__);
    return;
  }

  err = modbus_power001_lora_flow_task();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_power001_lora_flow_task failed", __LINE__,
             __func__);
    return;
  }
}
