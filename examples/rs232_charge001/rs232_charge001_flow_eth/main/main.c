#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "mt_ethernet.h"
#include "mt_memory_manage.h"
#include "mt_module_flow_manage.h"
#include "mt_module_http.h"
#include "mt_module_http_utils.h"
#include "mt_module_lora.h"
#include "mt_module_mqtt.h"
#include "mt_mqtt.h"
#include "mt_nvs_config.h"
#include "mt_nvs_storage.h"
#include "mt_smartconfig.h"
#include "mt_utils_session.h"

#include "rs232_charge001.h"
#include "rs232_charge001_module_flow.h"
#include "rs232_charge001_module_mqtt.h"
#include "rs232_dev.h"

// global define ==============================================================

static const char *TAG = "rs232_sim_air720h_http";

static int RS232_UART_NUM = 1;
static int RS232_RX_PIN = 15;
static int RS232_TX_PIN = 13;

static int LIGHT_PIN = 14;
static int LIGHT_PIN_ON_LEVEL = 0;
static int BTN_PIN = 35;
static int BTN_PIN_ON_LEVEL = 0;

// gloabal func ===============================================================

void app_main() {
  esp_err_t err = ESP_OK;
  mt_module_http_t *module_http = NULL;
  mt_module_flow_t *module_flow = NULL;
  mt_nvs_host_t *host_config = NULL;

  ESP_LOGI(TAG, "test begin");

  // nvs
  mt_nvs_init();

  mt_memory_manage_task(true);

  // network
  host_config = mt_nvs_config_get_host_config();
  if (host_config == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_get_host_config NULL", __LINE__,
             __func__);
    return;
  }
  if (strcmp(host_config->net_type, "wifi") == 0)
    mt_smartconfig_task(LIGHT_PIN, LIGHT_PIN_ON_LEVEL, BTN_PIN,
                        BTN_PIN_ON_LEVEL);
  else if (strcmp(host_config->net_type, "eth") == 0)
    mt_ethernet_task(LIGHT_PIN, LIGHT_PIN_ON_LEVEL);

  // charge001 init
  err = rs232_charge001_init(RS232_UART_NUM, RS232_RX_PIN, RS232_TX_PIN);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_init failed", __LINE__, __func__);
    return;
  }

  // metathings module http task
  module_http = mt_module_http_new(1);
  if (module_http == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_module_http_new failed", __LINE__, __func__);
    return;
  }

  // http task
  mt_module_http_task(module_http);

  // wait module session build
  while (module_http->module == NULL || module_http->module->id == NULL ||
         module_http->session_id == 0 ||
         module_http->module->deviceID == NULL) {
    vTaskDelay(2000 / portTICK_RATE_MS);
  }

  // metathings mqtt task
  mt_mqtt_init(1, module_http->module->id, module_http->session_id,
               module_http->module->deviceID, mt_module_mqtt_handle);
  rs232_charge001_module_mqtt_init();

  // metathings flow
  module_flow = mt_module_flow_new(1, 1, module_http);
  if (module_flow == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_new failed", __LINE__, __func__);
    return;
  }
  module_flow->push_frame_interval = 120 * 1000; // 120 second
  module_flow->ping_interval = 57 * 1000;        // 57 second

  // flow task
  mt_module_flow_task(module_flow);

  rs232_charge001_module_flow_task(module_flow);
}
