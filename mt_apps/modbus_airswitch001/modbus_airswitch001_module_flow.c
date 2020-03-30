#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "cache_airswitch001.h"
#include "modbus_airswitch001.h"
#include "modbus_airswitch001_module_flow.h"
#include "modbus_airswitch001_module_mqtt.h"
#include "modbus_airswitch001_notify.h"
#include "mt_module_flow.h"
#include "mt_module_http.h"
#include "mt_mqtt_lan.h"
#include "mt_mqtt_utils.h"
#include "mt_nvs_storage.h"

#include "google/protobuf/struct.pb-c.h"

#include "push_frame_to_flow.pb-c.h"

// global define ==============================================================

static const char *TAG = "MODULE_FLOW";

static int ADDR = 1;

static mt_module_flow_t *MODULE_FLOW = NULL;

// global func ================================================================

static void module_get_datas_process() {
  esp_err_t err = ESP_OK;
  Cache_t *cache = NULL;
  mt_module_flow_struct_group_t *group = NULL;
  int get_num = 0;
  bool *get_state = NULL;
  bool state[16];
  bool ctrl[16];
  double votage[16];
  double leakCurrent[16];
  double power[16];
  double temp[16];
  double current[16];
  int count = 0;
  char key[24] = "";

  // get switch num
  cache = cache_get();
  if (cache == NULL) {
    ESP_LOGE(TAG, "%4d %s cahe_get failed", __LINE__, __func__);
    return;
  }
  const int total_num = cache->num_master + cache->num_slaver;
  cache_free(cache);

  // get data process
  get_state = (bool *)malloc(total_num * sizeof(bool));
  for (int i = 1; i <= total_num; i++) {
    err = mt_airswitch001_get_datas(ADDR, i, &state[i], &ctrl[i], &votage[i],
                                    &leakCurrent[i], &power[i], &temp[i],
                                    &current[i]);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s addr:%d target:%d mt_airswitch001_get_datas failed",
               __LINE__, __func__, ADDR, i);
      get_state[i - 1] = false;
    } else {
      get_state[i - 1] = true;
      get_num++;
    }
  }

  // data struct
  int struct_size = 0;
  for (int i = 1; i <= total_num; i++) {
    if (get_state[i - 1] == true) {
      struct_size += 7;
    }
  }

  group = mt_module_flow_new_struct_group(struct_size);

  for (int i = 1; i <= total_num; i++) {
    if (get_state[i - 1] == true) {

      // state
      sprintf(key, "state%d", i);
      group->value[count]->key = malloc(strlen(key) + 1);
      memcpy(group->value[count]->key, key, strlen(key) + 1);
      group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE;
      group->value[count++]->bool_value = state[i];

      // ctrl
      sprintf(key, "ctrl%d", i);
      group->value[count]->key = malloc(strlen(key) + 1);
      memcpy(group->value[count]->key, key, strlen(key) + 1);
      group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE;
      group->value[count++]->bool_value = ctrl[i];

      // votage
      sprintf(key, "votage%d", i);
      group->value[count]->key = malloc(strlen(key) + 1);
      memcpy(group->value[count]->key, key, strlen(key) + 1);
      group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
      group->value[count++]->number_value = votage[i];

      // leakCurrent
      sprintf(key, "leakCurrent%d", i);
      group->value[count]->key = malloc(strlen(key) + 1);
      memcpy(group->value[count]->key, key, strlen(key) + 1);
      group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
      group->value[count++]->number_value = leakCurrent[i];

      // power
      sprintf(key, "power%d", i);
      group->value[count]->key = malloc(strlen(key) + 1);
      memcpy(group->value[count]->key, key, strlen(key) + 1);
      group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
      group->value[count++]->number_value = power[i];

      // temp
      sprintf(key, "temp%d", i);
      group->value[count]->key = malloc(strlen(key) + 1);
      memcpy(group->value[count]->key, key, strlen(key) + 1);
      group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
      group->value[count++]->number_value = temp[i];

      // current
      sprintf(key, "current%d", i);
      group->value[count]->key = malloc(strlen(key) + 1);
      memcpy(group->value[count]->key, key, strlen(key) + 1);
      group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
      group->value[count++]->number_value = current[i];
    }
  }

  // send msg
  err = mt_module_flow_sent_msg(MODULE_FLOW, group);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  // log
  for (int i = 1; i <= total_num; i++) {
    ESP_LOGI(TAG,
             " %4d %s sent success target %d state: %d, ctrl: %d,  "
             "votage: % .2f, leakCurrent: % .2f, power: % .2f, temp: % .2f, "
             "current: % .2f ",
             __LINE__, __func__, i, state[i], ctrl[i], votage[i],
             leakCurrent[i], power[i], temp[i], current[i]);
  }

EXIT:
  if (get_state != NULL)
    free(get_state);

  if (group != NULL)
    mt_module_flow_free_struct_group(group);

  return;
}

static void modbus_airswitch_flow_loop() {
  while (true) {
    module_get_datas_process();
    vTaskDelay(MODULE_FLOW->push_frame_interval / portTICK_RATE_MS);
  }
}

// global func ================================================================

esp_err_t module_notify_process(mt_module_flow_struct_group_t *group) {
  esp_err_t err = ESP_OK;

  err = mt_module_flow_sent_msg(MODULE_FLOW, group);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__, __func__);
    return err;
  }

  return err;
}

void modbus_airswitch_flow_task(mt_module_flow_t *module_flow) {
  MODULE_FLOW = module_flow;
  xTaskCreate((TaskFunction_t)modbus_airswitch_flow_loop,
              "MODBUS_AIRSWITCH_FLOW_TASK", 8 * 1024, NULL, 10, NULL);
  modbus_airswitch001_notify_task();
}
