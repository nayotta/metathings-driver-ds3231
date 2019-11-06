#include "stdint.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "modbus_gzpd800T.h"
#include "mt_mbtask.h"

// global define ==============================================================
static const char *TAG = "modbus_gzpd800T_example";

UCHAR RS485_PORT = 2;
ULONG RS485_BAUD = 9600;
eMBParity RS485_PARITY = MB_PAR_NONE;
int TX_PIN = 13;
int RX_PIN = 15;
int EN_PIN = 05;

// test func =================================================================
void test_get_data()
{
  esp_err_t err = ESP_OK;
  gzpd800T_4ch_data_t data;

  err = mt_gzpd800T_get_4ch_data(&data);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  ESP_LOGW(TAG,
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

void test_get_warn()
{
  esp_err_t err = ESP_OK;
  bool warn = false;

  for (int i = 0; i < 4; i++)
  {
    err = mt_gzpd800T_get_warn(i + 1, &warn);
    if (err != ESP_OK)
    {
      ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
      return;
    }

    ESP_LOGI(TAG, "%4d %s index:%d warn:%d success", __LINE__, __func__, i + 1,
             warn);
  }

  return;
}

// main func ==================================================================
void app_main()
{
  eMBErrorCode emb_ret = 0;

  ESP_LOGI(TAG, "test begin");

  emb_ret =
      modbus_init(RS485_PORT, RS485_BAUD, RS485_PARITY, TX_PIN, RX_PIN, EN_PIN);
  if (emb_ret != 0)
  {
    ESP_LOGE(TAG, "%4d modbus_init failed", __LINE__);
    return;
  }

  mt_vMBMaster_set_T35_interval(125);

  mt_modbus_task();

  while (1)
  {
    // test get data
    vTaskDelay(2000 / portTICK_RATE_MS);
    test_get_data();

    // test get warn
    // vTaskDelay(2000 / portTICK_RATE_MS);
    // test_get_warn();
  }
}