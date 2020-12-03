
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "modbus_ktc_n01.h"
#include "modbus_ktc_n01_module_flow.h"

#include "mt_module_flow.h"
#include "mt_module_http.h"
#include "mt_mqtt.h"
#include "mt_mqtt_utils.h"

#include "google/protobuf/struct.pb-c.h"
#include "push_frame_to_flow.pb-c.h"

// global define ==============================================================

static const char *TAG = "MODULE_KTC_N01_FLOW";

// static func ================================================================

static void module_ktc_n01_flow_loop(mt_module_flow_t *module_flow) {
  esp_err_t err = ESP_OK;
  mt_module_flow_struct_group_t *data = NULL;

  while (true) {
    data = modbus_ktc_n01_get_flow_data();
    if (data == NULL) {
      ESP_LOGE(TAG, "%4d %s rs232_charge001_get_flow_data failed", __LINE__,
               __func__);
      goto ERROR;
    }

    err = mt_module_flow_sent_msg(module_flow, data);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__,
               __func__);
    }

  ERROR:
    mt_module_flow_free_struct_group(data);
    data = NULL;

    vTaskDelay(module_flow->push_frame_interval / portTICK_PERIOD_MS);
  }
}

static void modbus_ktc_n01_flow_loop(mt_module_flow_t *module_flow) {
  while (true) {
    module_ktc_n01_flow_loop(module_flow);
    vTaskDelay(module_flow->push_frame_interval / portTICK_RATE_MS);
  }
}

// global fun =================================================================

void modbus_ktc_n01_module_flow_task(mt_module_flow_t *module_flow) {
  xTaskCreate((TaskFunction_t)modbus_ktc_n01_flow_loop, "MODULE_FLOW", 8 * 1024,
              module_flow, 10, NULL);
}
