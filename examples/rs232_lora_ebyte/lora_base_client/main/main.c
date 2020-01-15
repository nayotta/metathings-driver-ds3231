#include "esp_log.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "rs232_dev.h"
#include "rs232_lora_ebyte.h"

// global define ==============================================================

static const char *TAG = "example-rs232-lora-ebyte-client";

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
    rs232_lora_ebyte_data_t *ebyte_data = NULL;
    ebyte_data = rs232_lora_ebyte_new_data();
    ebyte_data->id = 10000;
    ebyte_data->type = 20000;
    ebyte_data->cmd = 30000;
    ebyte_data->handle = 40000;
    ebyte_data->len = 6;
    ebyte_data->data = malloc(ebyte_data->len);
    sprintf((char *)ebyte_data->data, "%s", "hello");

    rs232_lora_ebyte_sent(ebyte_data);
    vTaskDelay(5000 / portTICK_RATE_MS);
  }

  ESP_LOGI(TAG, "test end");
}
