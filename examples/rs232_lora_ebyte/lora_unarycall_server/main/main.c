#include "esp_log.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "rs232_dev.h"
#include "rs232_lora_ebyte.h"

// global define ==============================================================

static const char *TAG = "rs232_lora_ebyte_unarycall_client";

static int UART_NUM = 2;
static int BAUD_RATE = 9600;
static int RX_PIN = 9;
static int TX_PIN = 10;

// gloabal func ===============================================================

void recv_task_loop() {
  rs232_lora_ebyte_data_t *ebyte_recv = NULL;
  while (1) {
    ebyte_recv = rs232_lora_ebyte_recv();
    if (ebyte_recv == NULL) {
      ESP_LOGW(TAG, "%4d %s rs232_lora_ebyte_recv NULL", __LINE__, __func__);
    } else {
      ESP_LOGI(
          TAG, "%4d %s recv id=%d,type=%d,cmd=%dd,handle=%d,len=%d,data=%s",
          __LINE__, __func__, ebyte_recv->id, ebyte_recv->type, ebyte_recv->cmd,
          (int)ebyte_recv->handle, ebyte_recv->len, ebyte_recv->data);
    }

    vTaskDelay(10 / portTICK_RATE_MS);
  }
}

void app_main() {
  esp_err_t err = ESP_OK;
  QueueHandle_t handle = NULL;

  rs232_lora_ebyte_data_t *ebyte_data = NULL;
  ebyte_data = rs232_lora_ebyte_new_data();
  ebyte_data->id = 10000;
  ebyte_data->type = RS232_LORA_EBYTE_CMD_TYPE_UNARYCALL;
  ebyte_data->cmd = RS232_LORA_EBYTE_CMD_TYPE_FLOW;

  ESP_LOGI(TAG, "test begin");

  err = rs232_lora_ebyte_init(UART_NUM, RX_PIN, TX_PIN, BAUD_RATE);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_init failed", __LINE__, __func__);
    return;
  }

  xTaskCreate((TaskFunction_t)recv_task_loop, "recv_task", 8 * 1024, NULL, 10,
              NULL);

  handle = xQueueCreate(5, 100);
  ESP_LOGI(TAG, "%4d %s handle:%d", __LINE__, __func__, (int)handle);

  while (true) {

    ebyte_data->handle = handle;
    ebyte_data->data = (uint8_t *)"hello";
    ebyte_data->len = 6;
    err = rs232_lora_ebyte_sent(ebyte_data);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_sent failed", __LINE__, __func__);
    } else {
      ESP_LOGI(TAG, "%4d %s rs232_lora_ebyte_sent ok", __LINE__, __func__);
    }

    vTaskDelay(20000 / portTICK_RATE_MS);
  }

  ESP_LOGI(TAG, "test end");
}
