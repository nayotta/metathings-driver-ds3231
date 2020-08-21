#include "stdint.h"
#include "string.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "mt_memory_manage.h"

#include "modbus_gzpd800T.h"
#include "mt_mbtask.h"

#include "rs232_dev.h"
#include "rs232_lora_ebyte.h"

// global define ==============================================================
static const char *TAG = "modbus_gzpd800T_example";

UCHAR RS485_PORT = 2;
ULONG RS485_BAUD = 9600;
eMBParity RS485_PARITY = MB_PAR_NONE;
int TX_PIN = 13;
int RX_PIN = 15;
int EN_PIN = 05;

static int LORA_UART_NUM = 1;
static int LORA_BAUD_RATE = 9600;
static int LORA_RX_PIN = 4;
static int LORA_TX_PIN = 16;

// test func =================================================================
void test_get_data() {
  esp_err_t err = ESP_OK;
  gzpd800T_4ch_data_t data;

  err = modbus_gzpd800T_get_4ch_data(&data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  ESP_LOGI(TAG,
           "%4d %s \n"
           "index 1 amp:%8u freq:%8u power:%8u\n"
           "index 2 amp:%8u freq:%8u power:%8u\n"
           "index 3 amp:%8u freq:%8u power:%8u\n"
           "index 4 amp:%8u freq:%8u power:%8u\n",
           __LINE__, __func__, data.amp1, data.freq1, data.power1, data.amp2,
           data.freq2, data.power2, data.amp3, data.freq3, data.power3,
           data.amp4, data.freq4, data.power4);

  return;
}

void test_get_warn() {
  esp_err_t err = ESP_OK;
  bool warn = false;

  for (int i = 0; i < 4; i++) {
    err = modbus_gzpd800T_get_warn(i + 1, &warn);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
      return;
    }

    ESP_LOGI(TAG, "%4d %s index:%d warn:%d success", __LINE__, __func__, i + 1,
             warn);
  }

  return;
}

// main func ==================================================================
void app_main() {
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "test begin");

  mt_memory_manage_task(true);

  err = rs232_lora_ebyte_init(LORA_UART_NUM, LORA_RX_PIN, LORA_TX_PIN,
                              LORA_BAUD_RATE);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_init failed", __LINE__, __func__);
    return;
  }

  err = modbus_gzpd800T_init(RS485_PORT, TX_PIN, RX_PIN, EN_PIN);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d modbus_init failed", __LINE__);
    return;
  }

  gzpd800T_4ch_data_t data;
  rs232_lora_ebyte_data_t *ebyte_data = NULL;
  int interval = 30;
  while (1) {
    err = modbus_gzpd800T_get_4ch_data(&data);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
      goto ERROR;
    }

    ESP_LOGI(TAG,
             "%4d %s \n"
             "index 1 amp:%8u freq:%8u power:%8u\n"
             "index 2 amp:%8u freq:%8u power:%8u\n"
             "index 3 amp:%8u freq:%8u power:%8u\n"
             "index 4 amp:%8u freq:%8u power:%8u\n",
             __LINE__, __func__, data.amp1, data.freq1, data.power1, data.amp2,
             data.freq2, data.power2, data.amp3, data.freq3, data.power3,
             data.amp4, data.freq4, data.power4);

    ebyte_data = rs232_lora_ebyte_new_data();
    ebyte_data->id = 1;
    ebyte_data->type = 2;
    ebyte_data->cmd = 3;
    ebyte_data->handle = 4;
    ebyte_data->len = 12 * 4;
    ebyte_data->data = malloc(ebyte_data->len);

    memcpy(ebyte_data->data + 4 * 0, &data.amp1, 4);
    memcpy(ebyte_data->data + 4 * 1, &data.freq1, 4);
    memcpy(ebyte_data->data + 4 * 2, &data.power1, 4);
    memcpy(ebyte_data->data + 4 * 3, &data.amp2, 4);
    memcpy(ebyte_data->data + 4 * 4, &data.freq2, 4);
    memcpy(ebyte_data->data + 4 * 5, &data.power2, 4);
    memcpy(ebyte_data->data + 4 * 6, &data.amp3, 4);
    memcpy(ebyte_data->data + 4 * 7, &data.freq3, 4);
    memcpy(ebyte_data->data + 4 * 8, &data.power3, 4);
    memcpy(ebyte_data->data + 4 * 9, &data.amp4, 4);
    memcpy(ebyte_data->data + 4 * 10, &data.freq4, 4);
    memcpy(ebyte_data->data + 4 * 11, &data.power4, 4);

    rs232_lora_ebyte_sent(ebyte_data);
    rs232_lora_ebyte_free_data(ebyte_data);

  ERROR:
    vTaskDelay(interval / portTICK_RATE_MS);
  }
}
