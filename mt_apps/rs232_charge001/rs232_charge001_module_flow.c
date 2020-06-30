#include "rs232_charge001_module_flow.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "mt_module_flow.h"
#include "rs232_charge001.h"

// static define ==============================================================

static const char *TAG = "rs232_CHARGE001_MODULE_FLOW";

// static func ================================================================

static void rs232_charge001_flow_loop(mt_module_flow_t *module_flow) {
  esp_err_t err = ESP_OK;
  int64_t begin = esp_timer_get_time() / 1000000;
  int64_t end = 0;
  mt_module_flow_struct_group_t *data_save = NULL;
  mt_module_flow_struct_group_t *data = NULL;
  mt_module_flow_struct_group_t *data_diff = NULL;
  bool change = false;

  while (true) {
    data = rs232_charge001_get_states2();
    if (data == NULL) {
      ESP_LOGE(TAG, "%4d %s rs232_charge001_get_states2 failed", __LINE__,
               __func__);
    }

    err = mt_module_flow_sent_msg(module_flow, data);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__,
               __func__);
    }

    vTaskDelay(module_flow->push_frame_interval / portTICK_PERIOD_MS);
  }
}

// global func ================================================================

void rs232_charge001_module_flow_task(mt_module_flow_t *module_flow) {
  xTaskCreate((TaskFunction_t)rs232_charge001_flow_loop, "MODULE_FLOW",
              8 * 1024, module_flow, 10, NULL);
}

void modbus_charge001_module_flow_notify(uint8_t *buf, uint32_t size){
    
}
