
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "modbus_gzpd800T.h"
#include "modbus_gzpd800T_module_flow.h"

#include "mt_module_flow.h"
#include "mt_module_http.h"
#include "mt_mqtt_lan.h"
#include "mt_mqtt_utils.h"

#include "google/protobuf/struct.pb-c.h"
#include "push_frame_to_flow.pb-c.h"

// global define ==============================================================

static const char *TAG = "modbus_gzpd800t_module_flow";

// static func ================================================================

static void module_gzpd800T_get_data_process(mt_module_flow_t *module_flow) {
  esp_err_t err = ESP_OK;
  mt_module_flow_struct_group_t *group = NULL;
  gzpd800T_4ch_data_t data;
  bool warn[4];
  int count = 0;

  // get current data
  err = modbus_gzpd800T_get_4ch_data(&data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_gzpd800T_get_4ch_data failed", __LINE__,
             __func__);
    goto EXIT;
  }

  for (int i = 0; i < 4; i++) {
    err = modbus_gzpd800T_get_warn(i + 1, &warn[i]);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s modbus_gzpd800T_get_warn addr:%d failed", __LINE__,
               __func__, i);
      goto EXIT;
    }
  }

  // data struct
  int struct_size = 16;
  group = mt_module_flow_new_struct_group(struct_size);

  // amp1
  mt_module_flow_set_number_value(group->value[count++], "amp1", data.amp1);

  // amp2
  mt_module_flow_set_number_value(group->value[count++], "amp2", data.amp2);

  // amp3
  mt_module_flow_set_number_value(group->value[count++], "amp3", data.amp3);

  // amp4
  mt_module_flow_set_number_value(group->value[count++], "amp4", data.amp4);

  // freq1
  mt_module_flow_set_number_value(group->value[count++], "freq1", data.freq1);

  // freq2
  mt_module_flow_set_number_value(group->value[count++], "freq2", data.freq2);

  // freq3
  mt_module_flow_set_number_value(group->value[count++], "freq3", data.freq3);

  // freq4
  mt_module_flow_set_number_value(group->value[count++], "freq4", data.freq4);

  // power1
  mt_module_flow_set_number_value(group->value[count++], "power1", data.power1);

  // power2
  mt_module_flow_set_number_value(group->value[count++], "power2", data.power2);

  // power3
  mt_module_flow_set_number_value(group->value[count++], "power3", data.power3);

  // power4
  mt_module_flow_set_number_value(group->value[count++], "power4", data.power4);

  // warn1
  mt_module_flow_set_bool_value(group->value[count++], "warn1", warn[0]);

  // warn2
  mt_module_flow_set_bool_value(group->value[count++], "warn2", warn[1]);

  // warn3
  mt_module_flow_set_bool_value(group->value[count++], "warn3", warn[2]);

  // warn4
  mt_module_flow_set_bool_value(group->value[count++], "warn4", warn[3]);

  // send msg
  err = mt_module_flow_sent_msg(module_flow, group);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  // log
  ESP_LOGI(TAG, "%4d %s session:%s sent success", __LINE__, __func__,
           module_flow->session);
  for (int j = 0; j < group->size; j++) {
    switch (group->value[j]->type) {
    case GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE:
      ESP_LOGI(TAG, "%4d %s key:%s value:%d", __LINE__, __func__,
               group->value[j]->key, group->value[j]->bool_value);
      break;
    case GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE:
      ESP_LOGI(TAG, "%4d %s key:%s value:%f", __LINE__, __func__,
               group->value[j]->key, group->value[j]->number_value);
      break;
    default:
      ESP_LOGE(TAG, "%4d %s unexcept type", __LINE__, __func__);
    }
  }

EXIT:
  if (group != NULL)
    mt_module_flow_free_struct_group(group);

  return;
}

static void module_flow_loop(mt_module_flow_t *module_flow) {
  while (true) {
    module_gzpd800T_get_data_process(module_flow);
    vTaskDelay(module_flow->push_frame_interval / portTICK_RATE_MS);
  }
}

// global func ================================================================

void modbus_gzpd800T_module_flow_task(mt_module_flow_t *module_flow) {
  xTaskCreate((TaskFunction_t)module_flow_loop, "MODULE_FLOW", 8 * 1024,
              module_flow, 10, NULL);
}
