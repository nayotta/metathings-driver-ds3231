#include "stdint.h"
#include "string.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "mt_memory_manage.h"
#include "mt_nvs_storage.h"

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

  /*
    err = modbus_gzpd800T_get_4ch_data(&data);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
      return;
    }*/
  data.amp1 = 1;
  data.freq1 = 2;
  data.power1 = 3;
  data.amp2 = 4;
  data.freq2 = 5;
  data.power2 = 6;
  data.amp3 = 7;
  data.freq3 = 8;
  data.power3 = 9;
  data.amp4 = 10;
  data.freq4 = 11;
  data.power4 = 12;

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

  mt_nvs_init();

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

  vTaskDelay(2000 / portTICK_RATE_MS);

  gzpd800T_8ch_data_t data;
  rs232_lora_ebyte_data_t *ebyte_data = NULL;
  int interval = 30 * 1000;
  while (1) {

    err = modbus_gzpd800T_get_8ch_data(&data);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
      goto ERROR;
    }

    /*
    data.amp1 = 1;
    data.freq1 = 2;
    data.power1 = 3;
    data.amp2 = 4;
    data.freq2 = 5;
    data.power2 = 6;
    data.amp3 = 7;
    data.freq3 = 8;
    data.power3 = 9;
    data.amp4 = 10;
    data.freq4 = 11;
    data.power4 = 12;*/

    ESP_LOGI(TAG,
             "%4d %s \n"
             "index 1 amp:%8u freq:%8u power:%8u\n"
             "index 2 amp:%8u freq:%8u power:%8u\n"
             "index 3 amp:%8u freq:%8u power:%8u\n"
             "index 4 amp:%8u freq:%8u power:%8u\n"
             "index 5 amp:%8u freq:%8u power:%8u\n"
             "index 6 amp:%8u freq:%8u power:%8u\n"
             "index 7 amp:%8u freq:%8u power:%8u\n"
             "index 8 amp:%8u freq:%8u power:%8u\n",
             __LINE__, __func__, data.amp1, data.freq1, data.power1, data.amp2,
             data.freq2, data.power2, data.amp3, data.freq3, data.power3,
             data.amp4, data.freq4, data.power4, data.amp5, data.freq5,
             data.power5, data.amp6, data.freq6, data.power6, data.amp7,
             data.freq7, data.power7, data.amp8, data.freq8, data.power8);

    ebyte_data = rs232_lora_ebyte_new_data();
    ebyte_data->id = 1;
    ebyte_data->type = 2;
    ebyte_data->cmd = 3;
    ebyte_data->handle = 4;
    ebyte_data->len = 24 * 4;
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
    memcpy(ebyte_data->data + 4 * 12, &data.amp5, 4);
    memcpy(ebyte_data->data + 4 * 13, &data.freq5, 4);
    memcpy(ebyte_data->data + 4 * 14, &data.power5, 4);
    memcpy(ebyte_data->data + 4 * 15, &data.amp6, 4);
    memcpy(ebyte_data->data + 4 * 16, &data.freq6, 4);
    memcpy(ebyte_data->data + 4 * 17, &data.power6, 4);
    memcpy(ebyte_data->data + 4 * 18, &data.amp7, 4);
    memcpy(ebyte_data->data + 4 * 19, &data.freq7, 4);
    memcpy(ebyte_data->data + 4 * 20, &data.power7, 4);
    memcpy(ebyte_data->data + 4 * 21, &data.amp8, 4);
    memcpy(ebyte_data->data + 4 * 22, &data.freq8, 4);
    memcpy(ebyte_data->data + 4 * 23, &data.power8, 4);

    rs232_lora_ebyte_sent(ebyte_data);
    rs232_lora_ebyte_free_data(ebyte_data);

  ERROR:
    vTaskDelay(interval / portTICK_RATE_MS);
  }
}
