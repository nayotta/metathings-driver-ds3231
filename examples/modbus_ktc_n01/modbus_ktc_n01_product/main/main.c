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

#include "modbus_ktc_n01.h"

// global config ==============================================================

static const char *TAG = "ESP32_KTC_N01_PRODUCT_MAIN";

int LIGHT_PIN = 14;
int LIGHT_PIN_ON_LEVEL = 0;

int BTN_PIN = 35;
int BTN_PIN_ON_LEVEL = 0;

uint8_t UART_PORT = 2;
int TX_PIN = 13;
int RX_PIN = 15;
int EN_PIN = 05;

#define ETHERNET

// global func ================================================================

void app_main() {
  mt_module_http_t *module_http = NULL;
  mt_module_flow_t *module_flow = NULL;
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "%4d %s start", __LINE__, __func__);

  // nvs
  mt_nvs_init();
  mt_memory_manage_task(true);

  // ktc_n01 init
  err = modbus_ktc_n01_init(UART_PORT, TX_PIN, RX_PIN, EN_PIN, 1);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_ktc_n01_init failed", __LINE__, __func__);
    return;
  }

  // network
#ifdef ETHERNET
  mt_ethernet_task(LIGHT_PIN, LIGHT_PIN_ON_LEVEL);
#else
  mt_smartconfig_task(LIGHT_PIN, LIGHT_PIN_ON_LEVEL, BTN_PIN,
                      BTN_PIN_PRESS_LEVEL);
#endif

  // metathings module http task
  module_http = mt_module_http_new(1);
  if (module_http == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_module_http_new failed", __LINE__, __func__);
    return;
  }
  mt_module_http_task(module_http);

  // wait module session build
  while (module_http->module->id == NULL || module_http->session_id == 0 ||
         module_http->module->deviceID == NULL) {
    ESP_LOGW(TAG, "%4d %s wait module session building", __LINE__, __func__);
    vTaskDelay(2000 / portTICK_RATE_MS);
  }

  // metathings mqtt task
  mt_mqtt_init(1, module_http->module->id, module_http->session_id,
               module_http->module->deviceID, mt_module_mqtt_handle);
  modbus_ktc_n01_module_mqtt_init();

  // metathings flow manage
  module_flow = mt_module_flow_new(1, 1, module_http);
  if (module_flow == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_new failed", __LINE__, __func__);
    return;
  }
  module_flow->push_frame_interval = 120 * 1000; // 120s
  module_flow->ping_interval = 57 * 1000;        // 57 second
  mt_module_flow_task(module_flow);

  // flow task
  modbus_ktc_n01_module_flow_task(module_flow);
}
