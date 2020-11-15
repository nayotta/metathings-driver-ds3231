#include "stdint.h"

#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "rs232_charge001.h"
#include "rs232_charge001_lora_flow.h"
#include "rs232_charge001_lora_unarycall.h"
#include "rs232_lora_ebyte.h"

// global define ==============================================================
static const char *TAG = "example_rs232_charge001_lora_flow";

int RS232_PORT = 2;
int RX_PIN = 25;
int TX_PIN = 23;

int LORA_PORT = 1;
int LORA_TX = 10;
int LORA_RX = 9;
int LORA_BAUD = 9600;

// test
void test_func() {
  while (1) {
    uart_write_bytes(LORA_PORT, "1", 1);
    ESP_LOGE(TAG, "sent begin");
    vTaskDelay(5000 / portTICK_RATE_MS);
  }
}

// main func ==================================================================

void app_main() {
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "test begin");

  vTaskDelay(2000 / portTICK_RATE_MS);

  err = rs232_charge001_init(RS232_PORT, RX_PIN, TX_PIN);
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

  /*
    uint8_t buf[1000] = "";
    int len = 0;
    int last = 0;
    while (true) {
      len = uart_read_bytes(LORA_PORT, buf, 1, 1 / portTICK_RATE_MS);

      if (len != last) {
        ESP_LOGW(TAG, "recv:%lld", esp_timer_get_time() / 1000);
      }

      last = len;
    }*/

  err = rs232_charge001_lora_unarycall_task();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_lora_unarycall_task failed", __LINE__,
             __func__);
    return;
  }
}
