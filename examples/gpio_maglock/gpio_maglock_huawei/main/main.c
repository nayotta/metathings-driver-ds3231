#include "stdio.h"

#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "mt_ethernet.h"
#include "mt_memory_manage.h"
#include "mt_module_flow_manage.h"
#include "mt_module_http.h"
#include "mt_module_mqtt.h"
#include "mt_mqtt.h"
#include "mt_nvs_storage.h"
#include "mt_smartconfig.h"

#include "gpio_maglock001.h"
#include "gpio_maglock001_huawei_flow.h"

#include "huawei_mqtt.h"
#include "huawei_mqtt_manage.h"

// global config ==============================================================

static const char *TAG = "GPIO_MAGLOCK001_HUAWEI_MAIN";

int LIGHT_PIN = 14;
int LIGHT_PIN_ON_LEVEL = 0;

int BTN_PIN = 35;
int BTN_PIN_ON_LEVEL = 0;

#define ETHERNET

// global func ================================================================

void app_main() {
  mt_module_flow_t *module_flow = NULL;
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "%4d %s start", __LINE__, __func__);

  // nvs
  mt_nvs_init();
  mt_memory_manage_task(true);

  // maglock init
  err = gpio_maglock001_task(1);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_maglock001_task failed", __LINE__, __func__);
    return;
  }

  // network
#ifdef ETHERNET
  mt_ethernet_task(LIGHT_PIN, LIGHT_PIN_ON_LEVEL);
#else
  mt_smartconfig_task(LIGHT_PIN, LIGHT_PIN_ON_LEVEL, BTN_PIN, BTN_PIN_ON_LEVEL);
#endif

  // mqtt task
  huawei_mqtt_init(huawei_mqtt_manage_handle_process);

  // flow task
  module_flow = mt_module_flow_new(1, 1, NULL);
  if (module_flow == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_new failed", __LINE__, __func__);
    return;
  }
  module_flow->push_frame_interval = 120 * 1000; // 120s
  gpio_maglock001_huawei_flow_task(module_flow);
}
