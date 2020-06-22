#include "modbus_reclose001_module_flow.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "modbus_reclose001.h"
#include "mt_module_flow.h"

#include "esp_heap_trace.h"
#define NUM_RECORDS 100
static heap_trace_record_t trace_record[NUM_RECORDS];

// static define ==============================================================

static const char *TAG = "MODBUS_RECLOSE001_MODULE_FLOW";

// global func ================================================================

static void modbus_reclose001_flow_loop(mt_module_flow_t *module_flow) {
  esp_err_t err = ESP_OK;
  int64_t begin = esp_timer_get_time() / 1000000;
  int64_t end = 0;
  mt_module_flow_struct_group_t *data_save = NULL;
  mt_module_flow_struct_group_t *data = NULL;
  mt_module_flow_struct_group_t *data_diff = NULL;
  bool change = false;

  while (true) {
    // ESP_ERROR_CHECK(heap_trace_start(HEAP_TRACE_LEAKS));
    data = modbus_reclose001_get_data(1);
    if (data == NULL) {
      ESP_LOGE(TAG, "%4d %s modbus_reclose001_get_data failed", __LINE__,
               __func__);
      err = ESP_ERR_INVALID_RESPONSE;
      goto EXIT;
    }

    // mt_module_flow_free_struct_group(data);
    // ESP_ERROR_CHECK(heap_trace_stop());
    // heap_trace_dump();
    // continue;

    if (data_save == NULL) {
      data_save = data;
      continue;
    }

    data_diff = mt_module_flow_get_diff(data, data_save, &change);
    if (change == true) {
      ESP_LOGI(TAG, "%4d %s mt_module_flow_get_diff find diff", __LINE__,
               __func__);
      if (data_diff == NULL) {
        ESP_LOGE(TAG, "%4d %s data_diff NULL", __LINE__, __func__);
        err = ESP_ERR_INVALID_RESPONSE;
        goto EXIT;
      }
      data_diff = mt_module_flow_add_notify_to_group(data_diff);
      err = mt_module_flow_sent_msg(module_flow, data_diff);
      if (err != ESP_OK) {
        ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__,
                 __func__);
        err = ESP_OK;
        goto EXIT;
      }
      begin = esp_timer_get_time() / 1000000;
    }

    end = esp_timer_get_time() / 1000000;
    if (end - begin > (module_flow->push_frame_interval) / 1000) {
      ESP_LOGI(TAG, "%4d %s push frame", __LINE__, __func__);
      err = mt_module_flow_sent_msg_check_default(module_flow, data);
      if (err != ESP_OK) {
        ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__,
                 __func__);
        err = ESP_OK;
        goto EXIT;
      }
      begin = esp_timer_get_time() / 1000000;
    }

  EXIT:
    if (err != ESP_OK) {
      mt_module_flow_free_struct_group(data);
      data = NULL;
    } else {
      mt_module_flow_free_struct_group(data_save);
      data_save = data;
    }
    mt_module_flow_free_struct_group(data_diff);
    data_diff = NULL;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void modbus_reclose001_module_flow_task(mt_module_flow_t *module_flow) {
  ESP_ERROR_CHECK(heap_trace_init_standalone(trace_record, NUM_RECORDS));
  xTaskCreate((TaskFunction_t)modbus_reclose001_flow_loop, "MODULE_FLOW",
              8 * 1024, module_flow, 10, NULL);
}
