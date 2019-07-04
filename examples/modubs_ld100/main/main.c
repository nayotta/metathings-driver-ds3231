#include "stdint.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "modbus_ld100.h"

// global define ==============================================================
static const char *TAG = "modbus_ld100_example";

UCHAR RS485_PORT = 2;
ULONG RS485_BAUD = 9600;
eMBParity RS485_PARITY = MB_PAR_NONE;
int TX_PIN = 23;
int RX_PIN = 22;
int EN_PIN = 18;

int ADDR = 1;

// test func =================================================================
void test_ld100_get_addr()
{
  esp_err_t err = ESP_OK;
  bool state = true;

  err = mt_ld100_get_addr(ADDR, &state);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  ESP_LOGI(TAG, "%4d %s ret:%d", __LINE__, __func__, state);

  return;
}

void test_ld100_set_addr()
{
  esp_err_t err = ESP_OK;

  err = mt_ld100_set_addr(ADDR, ADDR);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  ESP_LOGI(TAG, "%4d %s success", __LINE__, __func__);

  return;
}

void test_ld100_get_state_and_relay()
{
  esp_err_t err = ESP_OK;
  bool state = true;
  bool relay = true;

  err = mt_ld100_get_state(ADDR, &state);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  err = mt_ld100_get_relay(ADDR, &relay);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  ESP_LOGI(TAG, "%4d %s state:%d, relay:%d", __LINE__, __func__, state, relay);

  return;
}

void test_ld100_set_relay(bool state)
{
  esp_err_t err = ESP_OK;

  err = mt_ld100_set_relay(ADDR, state);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  ESP_LOGI(TAG, "%4d %s to %d success", __LINE__, __func__, state);

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

  mt_modbus_task();

  // test get addr
  vTaskDelay(2000 / portTICK_RATE_MS);
  test_ld100_get_addr();

  // test set addr
  vTaskDelay(2000 / portTICK_RATE_MS);
  test_ld100_set_addr();

  // test set relay 1
  vTaskDelay(2000 / portTICK_RATE_MS);
  test_ld100_set_relay(true);

  // test set relay 0
  vTaskDelay(2000 / portTICK_RATE_MS);
  test_ld100_set_relay(false);

  ESP_LOGI(TAG, "test end");

  while (1)
  {
    test_ld100_get_state_and_relay();
  }
}