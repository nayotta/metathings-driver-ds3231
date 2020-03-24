#include "esp_log.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "rs232_dev.h"
#include "rs232_lora_ebyte.h"

// global define ==============================================================

static const char *TAG = "example-rs232-lora-ebyte-server";

static int UART_NUM = 2;
static int BAUD_RATE = 9600;
static int RX_PIN = 9;
static int TX_PIN = 10;

// gloabal func ===============================================================
void app_main() {
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "test begin");

  err = rs232_lora_ebyte_init(UART_NUM, RX_PIN, TX_PIN, BAUD_RATE);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_init failed", __LINE__, __func__);
    return;
  }

  err = rs232_lora_ebyte_task();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_task failed", __LINE__, __func__);
    return;
  }

  while (true) {
    vTaskDelay(5000 / portTICK_RATE_MS);
  }

  ESP_LOGI(TAG, "test end");
}
