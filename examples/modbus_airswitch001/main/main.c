#include "stdint.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "modbus_airswitch001.h"

// global define ==============================================================
static const char *TAG = "modbus_airswitch001_example";

UCHAR RS485_PORT = 2;
ULONG RS485_BAUD = 19200;
eMBParity RS485_PARITY = MB_PAR_NONE;
int TX_PIN = 23;
int RX_PIN = 22;
int EN_PIN = 18;

UCHAR ADDR = 1;

#define SWITCH_MAX 16
bool SWITCH_EXIST[SWITCH_MAX];

// test func =================================================================
void test_airswitch001_get_addrs()
{
  esp_err_t err = ESP_OK;
  USHORT addrs = 0;

REGET:
  err = mt_airswitch001_get_addrs(ADDR, &addrs);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    vTaskDelay(2000 / portTICK_RATE_MS);
    goto REGET;
    return;
  }

  ESP_LOGI(TAG, "%4d %s ret:%d", __LINE__, __func__, addrs);

  for (int i = 0; i < SWITCH_MAX; i++)
  {
    SWITCH_EXIST[i] = (bool)((addrs & (1 << i)) >> i);
  }
}

void test_airswitch001_get_ctrl(UCHAR target)
{
  esp_err_t err = ESP_OK;
  bool ctrl = false;

  err = mt_airswitch001_get_ctrl(ADDR, target, &ctrl);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  ESP_LOGI(TAG, "%4d %s ret:%d", __LINE__, __func__, ctrl);
}

void test_airswitch001_get_config(UCHAR target)
{
  esp_err_t err = ESP_OK;
  USHORT value = 0;

  // CFG_READ_VOTAGE_HIGH
  err = mt_airswitch001_get_config(ADDR, CFG_READ_VOTAGE_HIGH, target, &value);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }
  ESP_LOGI(TAG, "%4d %s CFG_READ_VOTAGE_HIGH:%d", __LINE__, __func__, value);

  // CFG_READ_VOTAGE_LOW
  err = mt_airswitch001_get_config(ADDR, CFG_READ_VOTAGE_LOW, target, &value);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }
  ESP_LOGI(TAG, "%4d %s CFG_READ_VOTAGE_LOW:%d", __LINE__, __func__, value);

  // CFG_READ_LEAK_CURRENT
  err = mt_airswitch001_get_config(ADDR, CFG_READ_LEAK_CURRENT, target, &value);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }
  ESP_LOGI(TAG, "%4d %s CFG_READ_LEAK_CURRENT:%d", __LINE__, __func__, value);

  // CFG_READ_POWER_HIGH
  err = mt_airswitch001_get_config(ADDR, CFG_READ_POWER_HIGH, target, &value);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }
  ESP_LOGI(TAG, "%4d %s CFG_READ_POWER_HIGH:%d", __LINE__, __func__, value);

  // CFG_READ_TEMP_HIGH
  err = mt_airswitch001_get_config(ADDR, CFG_READ_TEMP_HIGH, target, &value);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }
  ESP_LOGI(TAG, "%4d %s CFG_READ_TEMP_HIGH:%d", __LINE__, __func__, value);

  // CFG_READ_CURRENT_HIGH
  err = mt_airswitch001_get_config(ADDR, CFG_READ_CURRENT_HIGH, target, &value);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }
  ESP_LOGI(TAG, "%4d %s CFG_READ_CURRENT_HIGH:%d", __LINE__, __func__, value);

  // CFG_READ_MODEL
  err = mt_airswitch001_get_config(ADDR, CFG_READ_MODEL, target, &value);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }
  ESP_LOGI(TAG, "%4d %s CFG_READ_MODEL:%d", __LINE__, __func__, value);
}

void test_airswitch001_get_data(UCHAR target)
{
  esp_err_t err = ESP_OK;
  USHORT value = 0;

  // DATA_VOTAGE
  err = mt_airswitch001_get_data(ADDR, DATA_VOTAGE, target, &value);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }
  ESP_LOGI(TAG, "%4d %s DATA_VOTAGE:%d", __LINE__, __func__, value);

  // DATA_LEAK_CURRENT
  err = mt_airswitch001_get_data(ADDR, DATA_LEAK_CURRENT, target, &value);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }
  ESP_LOGI(TAG, "%4d %s DATA_LEAK_CURRENT:%d", __LINE__, __func__, value);

  // DATA_POWER
  err = mt_airswitch001_get_data(ADDR, DATA_POWER, target, &value);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }
  ESP_LOGI(TAG, "%4d %s DATA_POWER:%d", __LINE__, __func__, value);

  // DATA_TEMP
  err = mt_airswitch001_get_data(ADDR, DATA_TEMP, target, &value);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }
  ESP_LOGI(TAG, "%4d %s DATA_TEMP:%d", __LINE__, __func__, value);

  // DATA_CURRENT
  err = mt_airswitch001_get_data(ADDR, DATA_CURRENT, target, &value);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }
  ESP_LOGI(TAG, "%4d %s DATA_CURRENT:%d", __LINE__, __func__, value);

  // DATA_WARN
  err = mt_airswitch001_get_data(ADDR, DATA_WARN, target, &value);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }
  ESP_LOGI(TAG, "%4d %s DATA_WARN:%d", __LINE__, __func__, value);

  // DATA_QUALITY_L
  err = mt_airswitch001_get_data(ADDR, DATA_QUALITY_L, target, &value);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }
  ESP_LOGI(TAG, "%4d %s DATA_QUALITY_L:%d", __LINE__, __func__, value);

  // DATA_QUALITY_H
  err = mt_airswitch001_get_data(ADDR, DATA_QUALITY_H, target, &value);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }
  ESP_LOGI(TAG, "%4d %s DATA_QUALITY_H:%d", __LINE__, __func__, value);
}

void test_airswitch001_get_state(UCHAR target)
{
  esp_err_t err = ESP_OK;
  bool state = false;

  err = mt_airswitch001_get_state(ADDR, target, &state);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  ESP_LOGI(TAG, "%4d %s ret:%d", __LINE__, __func__, state);
}

// main func ==================================================================
void app_main()
{
  eMBErrorCode emb_ret = 0;

  ESP_LOGI(TAG, "test begin");

  emb_ret = modbus_airswitch001_init(RS485_PORT, RS485_BAUD, RS485_PARITY, TX_PIN,
                                     RX_PIN, EN_PIN);
  if (emb_ret != 0)
  {
    ESP_LOGE(TAG, "%4d modbus_init failed", __LINE__);
    return;
  }

  mt_modbus_airswitch001_task();

  ESP_LOGI(TAG, "test init end");

  test_airswitch001_get_addrs();

  while (1)
  {
    for (int i = 0; i < SWITCH_MAX; i++)
    {
      if (SWITCH_EXIST[i] == true)
      {
        ESP_LOGW(TAG, "\n\n==================switch:%d", i);
        test_airswitch001_get_ctrl(i);
        test_airswitch001_get_state(i);
        test_airswitch001_get_config(i);
        test_airswitch001_get_data(i);
      }
    }

    vTaskDelay(5000 / portTICK_RATE_MS);
  }
}
