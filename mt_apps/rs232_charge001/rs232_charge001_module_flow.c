#include "rs232_charge001_module_flow.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "mt_module_flow.h"
#include "rs232_charge001.h"

// static define ==============================================================

static const char *TAG = "rs232_CHARGE001_MODULE_FLOW";
static mt_module_flow_t *MODULE_FLOW = NULL;

// static func ================================================================

static void rs232_charge001_flow_loop() {
  esp_err_t err = ESP_OK;
  mt_module_flow_struct_group_t *data = NULL;

  while (true) {
    data = rs232_charge001_get_flow_data();
    if (data == NULL) {
      ESP_LOGE(TAG, "%4d %s rs232_charge001_get_flow_data failed", __LINE__,
               __func__);
    }

    err = mt_module_flow_sent_msg(MODULE_FLOW, data);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__,
               __func__);
    }

    mt_module_flow_free_struct_group(data);
    data = NULL;

    vTaskDelay(MODULE_FLOW->push_frame_interval / portTICK_PERIOD_MS);
  }
}

static void rs232_charge001_module_flow_sent_notify_state_loop(int32_t port) {
  esp_err_t err = ESP_OK;
  mt_module_flow_struct_group_t *data = NULL;

  // delay 1s wait open
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  data = rs232_charge001_get_port_notify_data(port);
  if (data == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_get_port_notify_data port:%d failed",
             __LINE__, __func__, port);
    return;
  }

  err = mt_module_flow_sent_msg(MODULE_FLOW, data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__, __func__);
  }

  mt_module_flow_free_struct_group(data);
  vTaskDelete(NULL);
}

// global func ================================================================

void rs232_charge001_module_flow_task(mt_module_flow_t *module_flow) {
  MODULE_FLOW = module_flow;
  xTaskCreate((TaskFunction_t)rs232_charge001_flow_loop, "MODULE_FLOW",
              8 * 1024, NULL, 10, NULL);
}

void rs232_charge001_module_notify_state_task(int32_t port) {
  xTaskCreate((TaskFunction_t)rs232_charge001_flow_loop, "MODULE_FLOW",
              8 * 1024, port, 10, NULL);
}
