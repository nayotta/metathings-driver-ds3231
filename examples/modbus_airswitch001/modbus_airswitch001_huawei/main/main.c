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

#include "modbus_airswitch001.h"

#include "huawei_mqtt.h"
#include "huawei_mqtt_flow.h"
#include "huawei_mqtt_manage.h"

// global config ==============================================================

static const char *TAG = "MODBUS_AIRSWITCH001_HUAWEI_MAIN";

int LIGHT_PIN = 14;
int LIGHT_PIN_ON_LEVEL = 0;

int BTN_PIN = 35;
int BTN_PIN_ON_LEVEL = 0;

uint8_t UART_PORT = 2;
int TX_PIN = 13;
int RX_PIN = 15;
int EN_PIN = 05;

// global func ================================================================

void app_main() {
  mt_module_flow_t *module_flow = NULL;
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "%4d %s start", __LINE__, __func__);

  // nvs
  mt_nvs_init();
  mt_memory_manage_task(true);

  // device init
  err = modbus_airswitch001_init(UART_PORT, TX_PIN, RX_PIN, EN_PIN);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_airswitch001_init failed", __LINE__, __func__);
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
  module_flow->flow_handle = modbus_airswitch001_get_flow_data;
  module_flow->poll_enable = true;
  module_flow->poll_interval = 3 * 1000; // 3s
  module_flow->poll_handle = modbus_airswitch001_get_has_changed;
  module_flow->poll_process = modbus_airswitch001_get_flow_notify_data;
  huawei_mqtt_flow_task(module_flow);
}
