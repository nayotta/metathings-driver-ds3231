
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "modbus_bits001.h"
#include "modbus_bits001_module_flow.h"

#include "mt_module_flow.h"
#include "mt_module_http.h"
#include "mt_mqtt.h"
#include "mt_mqtt_utils.h"

#include "google/protobuf/struct.pb-c.h"
#include "push_frame_to_flow.pb-c.h"

// global define ==============================================================

static const char *TAG = "MODULE_AEW100_FLOW";

// static func ================================================================

static void module_bits001_get_ht_data_process(mt_module_flow_t *module_flow) {
  esp_err_t err = ESP_OK;
  mt_module_flow_struct_group_t *group = NULL;
  double hum1;
  double temp1;
  double hum2;
  double temp2;
  int count = 0;

  // get temp data
  err = modbus_bits001_get_temp(1, &temp1);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_get_temp addr:%d failed", __LINE__,
             __func__, 1);
    goto EXIT;
  }
  err = modbus_bits001_get_temp(2, &temp2);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_get_temp addr:%d failed", __LINE__,
             __func__, 2);
    goto EXIT;
  }
  err = modbus_bits001_get_hum(1, &hum1);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_get_hum addr:%d failed", __LINE__,
             __func__, 1);
    goto EXIT;
  }
  err = modbus_bits001_get_hum(1, &hum2);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_get_hum addr:%d failed", __LINE__,
             __func__, 2);
    goto EXIT;
  }

  // data struct
  int struct_size = 4;
  group = mt_module_flow_new_struct_group(struct_size);

  // temp1
  mt_module_flow_set_number_value(group->value[count++], "temp1", temp1);

  // temp2
  mt_module_flow_set_number_value(group->value[count++], "temp2", temp2);

  // hum1
  mt_module_flow_set_number_value(group->value[count++], "hum1", hum1);

  // hum2
  mt_module_flow_set_number_value(group->value[count++], "hum2", hum2);

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
    ESP_LOGI(TAG, "%4d %s key:%s value:%f", __LINE__, __func__,
             group->value[j]->key, group->value[j]->number_value);
  }

EXIT:
  if (group != NULL)
    mt_module_flow_free_struct_group(group);

  return;
}

static void
module_bits001_get_frog_data_process(mt_module_flow_t *module_flow) {
  esp_err_t err = ESP_OK;
  mt_module_flow_struct_group_t *group = NULL;
  double frog1;
  double frog2;
  int count = 0;

  // get temp data
  err = modbus_bits001_get_frog(1, &frog1);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_get_frog addr:%d failed", __LINE__,
             __func__, 1);
    goto EXIT;
  }
  err = modbus_bits001_get_frog(2, &frog2);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_get_frog addr:%d failed", __LINE__,
             __func__, 2);
    goto EXIT;
  }

  // data struct
  int struct_size = 2;
  group = mt_module_flow_new_struct_group(struct_size);

  // frog1
  mt_module_flow_set_number_value(group->value[count++], "frog1", frog1);

  // frog2
  mt_module_flow_set_number_value(group->value[count++], "frog2", frog2);

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
    ESP_LOGI(TAG, "%4d %s key:%s value:%f", __LINE__, __func__,
             group->value[j]->key, group->value[j]->number_value);
  }

EXIT:
  if (group != NULL)
    mt_module_flow_free_struct_group(group);

  return;
}

static void modbus_bits001_ht_flow_loop(mt_module_flow_t *module_flow) {
  while (true) {
    module_bits001_get_ht_data_process(module_flow);
    vTaskDelay(module_flow->push_frame_interval / portTICK_RATE_MS);
  }
}

static void modbus_bits001_frog_flow_loop(mt_module_flow_t *module_flow) {
  while (true) {
    module_bits001_get_frog_data_process(module_flow);
    vTaskDelay(module_flow->push_frame_interval / portTICK_RATE_MS);
  }
}

// global fun =================================================================

void modbus_bits001_ht_module_flow_task(mt_module_flow_t *module_flow) {
  xTaskCreate((TaskFunction_t)modbus_bits001_ht_flow_loop, "MODULE_FLOW",
              8 * 1024, module_flow, 10, NULL);
}

void modbus_bits001_frog_module_flow_task(mt_module_flow_t *module_flow) {
  xTaskCreate((TaskFunction_t)modbus_bits001_frog_flow_loop, "MODULE_FLOW",
              8 * 1024, module_flow, 10, NULL);
}
