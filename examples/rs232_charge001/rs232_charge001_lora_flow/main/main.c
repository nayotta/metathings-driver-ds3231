#include "stdint.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "rs232_charge001.h"
#include "rs232_charge001_lora_flow.h"
#include "rs232_lora_ebyte.h"

// global define ==============================================================
static const char *TAG = "example_rs232_charge001_lora_flow";

int RS232_PORT = 2;
int TX_PIN = 13;
int RX_PIN = 15;

int LORA_PORT = 1;
int LORA_TX = 10;
int LORA_RX = 9;
int LORA_BAUD = 9600;

// main func ==================================================================

void app_main() {
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "test begin");

  err = rs232_charge001_init(RS232_PORT, TX_PIN, RX_PIN);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_init failed", __LINE__, __func__);
    return;
  }

  err = rs232_lora_ebyte_init(LORA_PORT, LORA_RX, LORA_TX, LORA_BAUD);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_init failed", __LINE__, __func__);
    return;
  }

  err = rs232_charge001_lora_flow_task();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_lora_flow_task failed", __LINE__,
             __func__);
    return;
  }
}
