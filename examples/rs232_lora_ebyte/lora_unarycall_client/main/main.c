#include "esp_log.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "rs232_dev.h"
#include "rs232_lora_ebyte.h"

// global define ==============================================================

static const char *TAG = "rs232_lora_ebyte_unarycall_client";

static int UART_NUM = 2;
static int BAUD_RATE = 9600;
static int RX_PIN = 9;
static int TX_PIN = 10;

// gloabal func ===============================================================
void app_main() {
  esp_err_t err = ESP_OK;
  rs232_lora_ebyte_data_t *ebyte_recv = NULL;

  rs232_lora_ebyte_data_t *ebyte_data = NULL;
  ebyte_data = rs232_lora_ebyte_new_data();
  ebyte_data->id = 10000;
  ebyte_data->type = 20000;
  ebyte_data->cmd = RS232_LORA_EBYTE_CMD_TYPE_UNARYCALL;

  ESP_LOGI(TAG, "test begin");

  err = rs232_lora_ebyte_init(UART_NUM, RX_PIN, TX_PIN, BAUD_RATE);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_init failed", __LINE__, __func__);
    return;
  }

  while (true) {
    ebyte_recv = rs232_lora_ebyte_recv();
    if (ebyte_recv == NULL) {
      ESP_LOGW(TAG, "%4d %s rs232_lora_ebyte_recv NULL", __LINE__, __func__);
    } else {
      if (ebyte_data->id == ebyte_recv->id) {
        ebyte_data->handle = ebyte_recv->handle;
        ebyte_data->data = (uint8_t *)"ack";
        ebyte_data->len = 4;
        err = rs232_lora_ebyte_sent(ebyte_data);
        if (err != ESP_OK) {
          ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_sent failed", __LINE__,
                   __func__);
        } else {
          ESP_LOGI(TAG, "%4d %s sent ack", __LINE__, __func__);
        }
      } else {
        ESP_LOGW(TAG, "%4d %s got id:%d except id:%d", __LINE__, __func__,
                 ebyte_recv->id, ebyte_data->id);
      }
    }

    vTaskDelay(10 / portTICK_RATE_MS);
  }

  ESP_LOGI(TAG, "test end");
}
