#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "rs232_dev.h"
#include "rs232_sim_air720h.h"
#include "rs232_sim_air720h_flow.h"

#include "mt_module_flow_manage.h"
#include "mt_module_http.h"
#include "mt_module_http_utils.h"
#include "mt_utils_session.h"

// global define ==============================================================

static const char *TAG = "rs232_sim_air720h_http";

static int UART_NUM = 2;
static int BAUD_RATE = 115200;
static int RX_PIN = 15;
static int TX_PIN = 13;
// new session may cause heartbeat 409, false better for test
#define NEW_SESSION false

// static func ================================================================

static void test_handle(char *topic, char *buf, int size) {
  printf("no handle\n");
}

// gloabal func ===============================================================

void app_main() {
  esp_err_t err = ESP_OK;
  mt_module_http_t *module_http = NULL;
  mt_module_flow_t *module_flow = NULL;

  ESP_LOGI(TAG, "test begin");

  // serial init
  err = rs232_sim_air720h_serial_init(UART_NUM, RX_PIN, TX_PIN, BAUD_RATE);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_serial_init failed", __LINE__,
             __func__);
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

  // mqtt task
  err = rs232_sim_air720h_serial_init(UART_NUM, RX_PIN, TX_PIN, BAUD_RATE);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_serial_init failed", __LINE__,
             __func__);
    return;
  }
  err = rs232_sim_air720h_mqtt_task(module_http->module->id,
                                    module_http->module->deviceID,
                                    module_http->session_id, test_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_mqtt_task failed", __LINE__,
             __func__);
    return;
  }

  // flow task
  err = rs232_sim_air720h_flow_task(module_http);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_flow_task failed", __LINE__,
             __func__);
    return;
  }

  ESP_LOGI(TAG, "test end");
}
