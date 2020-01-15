#include "esp_log.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "rs232_dev.h"
#include "rs232_lora_ebyte.h"
#include "rs232_lora_ebyte_flow_manage.h"

#include "mt_module_flow_manage.h"
#include "mt_module_mqtt.h"

// global define ==============================================================

static const char *TAG = "example-rs232-lora-ebyte-server";

static int UART_NUM = 2;
static int BAUD_RATE = 9600;
static int RX_PIN = 9;
static int TX_PIN = 10;

// gloabal func ===============================================================

void app_main() {
  esp_err_t err = ESP_OK;
  QueueHandle_t handle = NULL;
  mt_module_mqtt_msg_t *mqtt_msg = malloc(sizeof(mt_module_mqtt_msg_t));
  mt_module_flow_t *flow = malloc(sizeof(mt_module_flow_t));

  ESP_LOGI(TAG, "test begin");

  err = rs232_lora_ebyte_init(UART_NUM, RX_PIN, TX_PIN, BAUD_RATE);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_init failed", __LINE__, __func__);
    return;
  }

  ESP_LOGI(TAG, "%4d %s rs232_lora_ebyte_task", __LINE__, __func__);
  err = rs232_lora_ebyte_task();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_task failed", __LINE__, __func__);
    return;
  }

  handle = xQueueCreate(5, sizeof(mt_module_mqtt_msg_t *));

  ESP_LOGI(TAG, "%4d %s create flow", __LINE__, __func__);
  // create flow
  flow->module_http = malloc(sizeof(mt_module_http_t));
  flow->module_http->module = malloc(sizeof(module_t));
  flow->module_http->module->deviceID = "12345678";
  flow->flow = malloc(sizeof(flow_t));
  flow->flow->name = "name";
  flow->session = "87654321";

  // debug log
  ESP_LOGI(TAG, "free=%d", esp_get_free_heap_size());

  // add flow manage
  mt_module_flow_manage_add(flow);

  // debug
  /*
  while (true) {
    ESP_LOGI(TAG, "free=%d", esp_get_free_heap_size());
    vTaskDelay(2000 / portTICK_RATE_MS);
  }*/

  // create ebyte flow manage
  ESP_LOGI(TAG, "%4d %s rs232_lora_ebyte_flow_manage_add", __LINE__, __func__);
  err = rs232_lora_ebyte_flow_manage_add(flow, 10000, handle);
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
    }
  }

  ESP_LOGI(TAG, "test end");
}
