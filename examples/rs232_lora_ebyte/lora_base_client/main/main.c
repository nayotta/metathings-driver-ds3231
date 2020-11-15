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
static int RX_PIN = 4;
static int TX_PIN = 16;

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
    ebyte_data->len = 1600;
    ebyte_data->data = malloc(ebyte_data->len);
    sprintf(
        (char *)ebyte_data->data, "%s",
        "hello1234hello1234hello1234hello1234hello1234hello1234hello1234hel"
        "lo1234hello1234hello1234hello1234hello1234hello1234hello1234hello1"
        "234hello1234hello1234hello1234hello1234hello1234hello1234hello1234"
        "hello1234hello1234hello1234hello1234hello1234hello1234hello1234hel"
        "lo1234hello1234hello1234hello1234hello1234hello1234hello1234hello1234h"
        "ello1234hello1234hello1234hello1234hello1234hello1234hello1234hello123"
        "4hello1234hello1234hello1234hello1234hello1234hello1234hello1234hello1"
        "234hello1234hello1234hello1234hello1234hello1234hello1234hello1234hell"
        "o1234hello1234hello1234hello1234hello1234hello1234hello1234hello1234he"
        "llo1234hello1234hello1234hello1234hello1234hello1234hello1234hello1234"
        "hello1234hello1234hello1234hello1234hello1234hello1234hello1234hello12"
        "34hello1234hello1234hello1234hello1234hello1234hello1234hello1234hello"
        "1234hello1234hello1234hello1234hello1234hello1234hello1234hello1234hel"
        "lo1234hello1234hello1234hello1234hello1234hello1234hello1234hello1234h"
        "ello1234hello1234hello1234hello1234hello1234hello1234hello1234hello123"
        "4hello1234hello1234hello1234hello1234hello1234hello1234hello1234hello1"
        "234hello1234hello1234hello1234hello1234hello1234hello1234hello1234hell"
        "o1234hello1234hello1234hello1234hello1234hello1234hello1234hello1234he"
        "llo1234hello1234hello1234hello1234hello1234hello1234hello1234hello1234"
        "hello1234hello1234hello1234hello1234hello1234hello1234hello1234hello12"
        "34hello1234hello1234hello1234hello1234hello1234hello1234hello1234hello"
        "1234hello1234hello1234hello1234hello1234hello1234hello1234hello1234hel"
        "lo1234hello1234hello1234hello1234hello1234hello1234hello1234hello123"
        "4123456");

    rs232_lora_ebyte_sent(ebyte_data);
    vTaskDelay(5000 / portTICK_RATE_MS);
  }

  ESP_LOGI(TAG, "test end");
}
