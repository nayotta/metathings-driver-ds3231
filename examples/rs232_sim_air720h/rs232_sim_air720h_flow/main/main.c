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

#include "mt_module_flow_manage.h"
#include "mt_module_http.h"
#include "mt_module_http_utils.h"
#include "mt_module_lora.h"
#include "mt_module_mqtt.h"
#include "mt_utils_session.h"

// global define ==============================================================

static const char *TAG = "rs232_sim_air720h_http";

static int SIM_UART_NUM = 2;
static int SIM_BAUD_RATE = 115200;
static int SIM_RX_PIN = 15;
static int SIM_TX_PIN = 13;

static int LORA_UART_NUM = 1;
static int LORA_BAUD_RATE = 9600;
static int LORA_RX_PIN = 16;
static int LORA_TX_PIN = 17;

// new session may cause heartbeat 409, false better for test
#define NEW_SESSION false

// gloabal func ===============================================================

void app_main() {
  esp_err_t err = ESP_OK;
  mt_module_http_t *module_http = NULL;
  mt_module_flow_t *module_flow = NULL;

  ESP_LOGI(TAG, "test begin");

  // serial init
  err = rs232_sim_air720h_serial_init(SIM_UART_NUM, SIM_RX_PIN, SIM_TX_PIN,
                                      SIM_BAUD_RATE);
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

  err = rs232_sim_air720h_mqtt_task(
      module_http->module->id, module_http->module->deviceID,
      module_http->session_id, mt_module_lora_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_mqtt_task failed", __LINE__,
             __func__);
    return;
  }

  // metathings flow
  module_flow = mt_module_flow_new(1, 1, module_http);
  if (module_flow == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_new failed", __LINE__, __func__);
    return;
  }
  module_flow->push_frame_interval = 300 * 1000; // 300 second
  module_flow->ping_interval = 57 * 1000;        // 57 second

  // flow task
  err = rs232_sim_air720h_flow_task(module_flow);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_flow_task failed", __LINE__,
             __func__);
    return;
  }

  // lora task
  err = rs232_lora_ebyte_init(LORA_UART_NUM, LORA_RX_PIN, LORA_TX_PIN,
                              LORA_BAUD_RATE);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_init failed", __LINE__, __func__);
    return;
  }
  err = rs232_lora_ebyte_task();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_task failed", __LINE__, __func__);
    return;
  }
  QueueHandle_t handle = NULL;
  mt_module_mqtt_msg_t *mqtt_msg = malloc(sizeof(mt_module_mqtt_msg_t));
  handle = xQueueCreate(5, sizeof(mt_module_mqtt_msg_t *));
  mt_module_flow_manage_add(module_flow);
  ESP_LOGI(TAG, "%4d %s rs232_lora_ebyte_flow_manage_add", __LINE__, __func__);
  err = rs232_lora_ebyte_flow_manage_add(module_flow, 1, handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_flow_manage_add failed", __LINE__,
             __func__);
    return;
  }

  ESP_LOGI(TAG, "%4d %s sent loop", __LINE__, __func__);
  while (true) {
    if (xQueueReceive(handle, &mqtt_msg, 1000 / portTICK_RATE_MS)) {
      ESP_LOGW(TAG, "%4d %s get mqtt msg,buf_size=%d,topic=%s,buf=%s", __LINE__,
               __func__, mqtt_msg->buf_size, mqtt_msg->topic, mqtt_msg->buf);
      uint8_t *buf = NULL;
      int size = 0;
      buf = mt_module_flow_repack_frame_with_session(
          mqtt_msg->buf, mqtt_msg->buf_size, module_flow->session, &size);
      rs232_sim_air720h_mqtt_pub(mqtt_msg->topic, buf, size);
      mt_module_mqtt_free_msg(mqtt_msg);
    }
  }

  ESP_LOGI(TAG, "test end");
}
