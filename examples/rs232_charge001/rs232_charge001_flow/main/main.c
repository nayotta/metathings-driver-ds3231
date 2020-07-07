#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "rs232_dev.h"
#include "rs232_lora_ebyte.h"
#include "rs232_lora_ebyte_flow_manage.h"
#include "rs232_sim_air720h.h"
#include "rs232_sim_air720h_flow.h"

#include "mt_memory_manage.h"
#include "mt_module_flow_manage.h"
#include "mt_module_http.h"
#include "mt_module_http_utils.h"
#include "mt_module_lora.h"
#include "mt_module_mqtt.h"
#include "mt_nvs_storage.h"
#include "mt_utils_session.h"

#include "rs232_charge001.h"
#include "rs232_charge001_module_flow.h"
#include "rs232_charge001_module_mqtt.h"

// global define ==============================================================

static const char *TAG = "rs232_sim_air720h_http";

static int SIM_UART_NUM = 2;
static int SIM_BAUD_RATE = 115200;
static int SIM_RX_PIN = 15;
static int SIM_TX_PIN = 13;

static int RS232_UART_NUM = 1;
static int RS232_RX_PIN = 16;
static int RS232_TX_PIN = 17;

// gloabal func ===============================================================

void app_main() {
  esp_err_t err = ESP_OK;
  mt_module_http_t *module_http = NULL;
  mt_module_flow_t *module_flow = NULL;

  ESP_LOGI(TAG, "test begin");

  // nvs
  mt_nvs_init();

  mt_memory_manage_task(true);

  // serial init
  err = rs232_sim_air720h_serial_init(SIM_UART_NUM, SIM_RX_PIN, SIM_TX_PIN,
                                      SIM_BAUD_RATE);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_serial_init failed", __LINE__,
             __func__);
    return;
  }

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
  err = rs232_sim_air720h_http_task(module_http);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_task failed", __LINE__,
             __func__);
    return;
  }

  // wait module session build
  while (module_http->module->id == NULL || module_http->session_id == 0 ||
         module_http->module->deviceID == NULL) {
    vTaskDelay(2000 / portTICK_RATE_MS);
  }

  err = rs232_sim_air720h_mqtt_task(
      1, module_http->module->id, module_http->module->deviceID,
      module_http->session_id, mt_module_mqtt_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_mqtt_task failed", __LINE__,
             __func__);
    return;
  }
  rs232_charge001_module_mqtt_init();

  // metathings flow
  module_flow = mt_module_flow_new(1, 1, module_http);
  if (module_flow == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_new failed", __LINE__, __func__);
    return;
  }
  module_flow->push_frame_interval = 120 * 1000; // 120 second
  module_flow->ping_interval = 57 * 1000;       // 57 second

  // flow task
  err = rs232_sim_air720h_flow_task(module_flow);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_flow_task failed", __LINE__,
             __func__);
    return;
  }

  rs232_charge001_module_flow_task(module_flow);
}
