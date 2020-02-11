#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "cache_airswitch001.h"
#include "modbus_airswitch001.h"
#include "modbus_airswitch001_module_flow.h"
#include "modbus_airswitch001_module_mqtt.h"
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
static int num_master = 1;
static int num_slaver = 2;

// global func ================================================================

static void module_get_datas_process(mt_module_flow_t *module_flow) {
  esp_err_t err = ESP_OK;
  char req_topic[256] = "";
  int frame_req_size = 0;
  uint8_t *frame_req_buf = NULL;
  Cache_t *cache = NULL;

  cache = cache_get_and_check(num_master, num_slaver);
  if (cache == NULL) {
    ESP_LOGE(TAG, "%4d %s cache_get_and_check failed", __LINE__, __func__);
    return;
  }

  const int total_num = cache->num_master + cache->num_slaver;
  cache_free(cache);

  bool state[16];
  bool ctrl[16];
  double votage[16];
  double leakCurrent[16];
  double power[16];
  double temp[16];
  double current[16];

  if (module_flow == NULL) {
    ESP_LOGE(TAG, "%4d %s module_flow NULL", __LINE__, __func__);
    return;
  } else {
    if (module_flow->session == NULL) {
      ESP_LOGE(TAG, "%4d %s module_flow->session NULL", __LINE__, __func__);
      return;
    }

    if (module_flow->module_http == NULL) {
      ESP_LOGE(TAG, "%4d %s module_flow->module_http NULL", __LINE__, __func__);
      return;
    } else {
      if (module_flow->module_http->module == NULL) {
        ESP_LOGE(TAG, "%4d %s module_flow->module_http->module NULL", __LINE__,
                 __func__);
        return;
      } else {
        if (module_flow->module_http->module->deviceID == NULL) {
          ESP_LOGE(TAG,
                   "%4d %s module_flow->module_http->module->deviceID NULL",
                   __LINE__, __func__);
          return;
        }
      }
    }
  }

  sprintf(req_topic, "mt/devices/%s/flow_channel/sessions/%s/upstream",
          module_flow->module_http->module->deviceID, module_flow->session);

  // get data process
  int get_num = 0;
  bool *get_state = NULL;
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
  module_struct_group_t data_struct = {
      .size = struct_size,
      .value = NULL,
  };

  data_struct.value = malloc(data_struct.size * sizeof(module_struct_t *));
  for (int i = 0; i < data_struct.size; i++) {
    data_struct.value[i] = mt_module_flow_struct_new();
  }

  int count = 0;

  char key[24] = "";
  for (int i = 1; i <= total_num; i++) {
    if (get_state[i - 1] == true) {

      // state
      sprintf(key, "state%d", i);
      data_struct.value[count]->key = malloc(strlen(key) + 1);
      memcpy(data_struct.value[count]->key, key, strlen(key) + 1);
      data_struct.value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE;
      data_struct.value[count++]->bool_value = state[i];

      // ctrl
      sprintf(key, "ctrl%d", i);
      data_struct.value[count]->key = malloc(strlen(key) + 1);
      memcpy(data_struct.value[count]->key, key, strlen(key) + 1);
      data_struct.value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE;
      data_struct.value[count++]->bool_value = ctrl[i];

      // votage
      sprintf(key, "votage%d", i);
      data_struct.value[count]->key = malloc(strlen(key) + 1);
      memcpy(data_struct.value[count]->key, key, strlen(key) + 1);
      data_struct.value[count]->type =
          GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
      data_struct.value[count++]->number_value = votage[i];

      // leakCurrent
      sprintf(key, "leakCurrent%d", i);
      data_struct.value[count]->key = malloc(strlen(key) + 1);
      memcpy(data_struct.value[count]->key, key, strlen(key) + 1);
      data_struct.value[count]->type =
          GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
      data_struct.value[count++]->number_value = leakCurrent[i];

      // power
      sprintf(key, "power%d", i);
      data_struct.value[count]->key = malloc(strlen(key) + 1);
      memcpy(data_struct.value[count]->key, key, strlen(key) + 1);
      data_struct.value[count]->type =
          GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
      data_struct.value[count++]->number_value = power[i];

      // temp
      sprintf(key, "temp%d", i);
      data_struct.value[count]->key = malloc(strlen(key) + 1);
      memcpy(data_struct.value[count]->key, key, strlen(key) + 1);
      data_struct.value[count]->type =
          GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
      data_struct.value[count++]->number_value = temp[i];

      // current
      sprintf(key, "current%d", i);
      data_struct.value[count]->key = malloc(strlen(key) + 1);
      memcpy(data_struct.value[count]->key, key, strlen(key) + 1);
      data_struct.value[count]->type =
          GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
      data_struct.value[count++]->number_value = current[i];
    }
  }

  // marshall data
  frame_req_buf = mt_module_flow_pack_frame(&data_struct, module_flow->session,
                                            &frame_req_size);
  if (frame_req_buf == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_pack_frame error", __LINE__, __func__);
    goto EXIT;
  }

  // mqtt pub
  err = mqtt_pub_msg(req_topic, frame_req_buf, frame_req_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    return;
  }

  // log
  for (int i = 1; i <= total_num; i++) {
    ESP_LOGI(TAG,
             " %4d %s session:%s sent success target %d state: %d, ctrl: %d,  "
             "votage: % .2f, leakCurrent: % .2f, power: % .2f, temp: % .2f, "
             "current: % .2f ",
             __LINE__, __func__, module_flow->session, i, state[i], ctrl[i],
             votage[i], leakCurrent[i], power[i], temp[i], current[i]);
  }

EXIT:
  mt_module_flow_struct_free(&data_struct);
  if (get_state != NULL) {
    free(get_state);
  }
  if (frame_req_buf != NULL) {
    free(frame_req_buf);
  }
  return;
}

static void module_flow_loop(mt_module_flow_t *module_flow) {
  while (true) {
    module_get_datas_process(module_flow);
    vTaskDelay(module_flow->push_frame_interval / portTICK_RATE_MS);
  }
}

void module_flow_task(mt_module_flow_t *module_flow, char *task_name) {
  xTaskCreate((TaskFunction_t)module_flow_loop, task_name, 8 * 1024,
              module_flow, 10, NULL);
}
