#include "stdint.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "modbus_airswitch001.h"
#include "mt_nvs_storage.h"

// global define ==============================================================
static const char *TAG = "modbus_airswitch001_example";

int TX_PIN = 13;
int RX_PIN = 15;
int EN_PIN = 05;

UCHAR ADDR = 1;

int EXAMPLE_NUM_MASTER = 1;
int EXAMPLE_NUM_SLAVER = 1;

// test func =================================================================
void test_airswitch001_get_addrs() {
  esp_err_t err = ESP_OK;
  USHORT addrs = 0;

REGET:
  err = mt_airswitch001_get_addrs(ADDR, &addrs);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    vTaskDelay(2000 / portTICK_RATE_MS);
    goto REGET;
    return;
  }

  ESP_LOGI(TAG, "%4d %s ret:%d", __LINE__, __func__, addrs);
}

void test_airswitch001_get_config(UCHAR target) {
  esp_err_t err = ESP_OK;
  double votage_high = 0;
  double votage_low = 0;
  double leak_current = 0;
  double power = 0;
  double temp = 0;
  double current = 0;

  err = mt_airswitch001_get_configs(ADDR, target, &votage_high, &votage_low,
                                    &leak_current, &power, &temp, &current);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  ESP_LOGI(TAG, "%4d %s cfg_votage_high :%f", __LINE__, __func__, votage_high);
  ESP_LOGI(TAG, "%4d %s cfg_votage_low  :%f", __LINE__, __func__, votage_low);
  ESP_LOGI(TAG, "%4d %s cfg_leak_current:%f", __LINE__, __func__, leak_current);
  ESP_LOGI(TAG, "%4d %s cfg_power       :%f", __LINE__, __func__, power);
  ESP_LOGI(TAG, "%4d %s cfg_temp        :%f", __LINE__, __func__, temp);
  ESP_LOGI(TAG, "%4d %s cfg_current     :%f", __LINE__, __func__, current);
}

void test_airswitch001_get_data(UCHAR target) {
  esp_err_t err = ESP_OK;
  bool state = false;
  bool ctrl = false;
  double votage = 0;
  double leak_current = 0;
  double power = 0;
  double temp = 0;
  double current = 0;

  err = mt_airswitch001_get_datas(ADDR, target, &state, &ctrl, &votage,
                                  &leak_current, &power, &temp, &current);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_airswitch001_get_datas failed code:%d", __LINE__,
             __func__, err);
    return;
  }

  ESP_LOGI(TAG, "%4d %s state       :%d", __LINE__, __func__, state);
  ESP_LOGI(TAG, "%4d %s ctrl        :%d", __LINE__, __func__, ctrl);
  ESP_LOGI(TAG, "%4d %s votage      :%f", __LINE__, __func__, votage);
  ESP_LOGI(TAG, "%4d %s leak_current:%f", __LINE__, __func__, leak_current);
  ESP_LOGI(TAG, "%4d %s power       :%f", __LINE__, __func__, power);
  ESP_LOGI(TAG, "%4d %s temp        :%f", __LINE__, __func__, temp);
  ESP_LOGI(TAG, "%4d %s current     :%f", __LINE__, __func__, current);
}

// main func ==================================================================
void app_main() {
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "test begin");

  mt_nvs_init();

  if (mt_nvs_write_int32_config("mod_1_master", EXAMPLE_NUM_MASTER) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_write_int32_config mod_1_master failed",
             __LINE__, __func__);
    return;
  }

  if (mt_nvs_write_int32_config("mod_1_slaver", EXAMPLE_NUM_SLAVER) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_write_int32_config mod_1_slaver failed",
             __LINE__, __func__);
    return;
  }

  err = mt_modbus_airswitch001_task(TX_PIN, RX_PIN, EN_PIN);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_modbus_airswitch001_task failed", __LINE__,
             __func__);
    return;
  }
  vTaskDelay(2000 / portTICK_RATE_MS);

  ESP_LOGI(TAG, "test init end");

  test_airswitch001_get_addrs();

  while (1) {
    for (int i = 1; i <= EXAMPLE_NUM_MASTER + EXAMPLE_NUM_SLAVER; i++) {
      ESP_LOGW(TAG, "\n\n==================switch:%d", i);
      test_airswitch001_get_config(i);
      test_airswitch001_get_data(i);
    }

    vTaskDelay(5000 / portTICK_RATE_MS);
  }
}
