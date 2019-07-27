#include "stdint.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "modbus_aew100.h"

// global define ==============================================================
static const char *TAG = "modbus_aew100_example";

UCHAR RS485_PORT = 2;
ULONG RS485_BAUD = 9600;
eMBParity RS485_PARITY = MB_PAR_NONE;
int TX_PIN = 23;
int RX_PIN = 22;
int EN_PIN = 18;

UCHAR ADDR = 27;

// test func =================================================================
void test_aew100_get_current()
{
  esp_err_t err = ESP_OK;
  bool state = true;
  double currentA = 0;
  double currentB = 0;
  double currentC = 0;

  err = mt_aew100_get_current(ADDR, &currentA, &currentB, &currentC);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  ESP_LOGI(TAG, "%4d %s ret:%d", __LINE__, __func__, state);

  return;
}

// main func ==================================================================
void app_main()
{
  eMBErrorCode emb_ret = 0;

  ESP_LOGI(TAG, "test begin");

  emb_ret =
      modbus_aew100_init(RS485_PORT, RS485_BAUD, RS485_PARITY, TX_PIN, RX_PIN, EN_PIN);
  if (emb_ret != 0)
  {
    ESP_LOGE(TAG, "%4d modbus_init failed", __LINE__);
    return;
  }

  mt_modbus_aew100_task();

  ESP_LOGI(TAG, "test end");

  while (1)
  {
    test_aew100_get_current();
    vTaskDelay(2000 / portTICK_RATE_MS);
  }
}