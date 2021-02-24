#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gpio_maglock001.h"
#include "gpio_maglock001_module_flow.h"
#include "gpio_maglock001_module_mqtt.h"
#include "mt_module_flow.h"
#include "mt_module_http.h"

#include "google/protobuf/struct.pb-c.h"
#include "push_frame_to_flow.pb-c.h"

// global define ==============================================================

static const char *TAG = "MODULE_FLOW";

// global func ================================================================

static void
gpio_maglock001_module_get_state_process(mt_module_flow_t *module_flow) {
  esp_err_t err = ESP_OK;
  mt_module_flow_struct_group_t *group = NULL;
  int port_num = 0;
  int count = 0;
  char key[24] = "";

  if (module_flow == NULL) {
    ESP_LOGE(TAG, "%4d %s module_flow NULL", __LINE__, __func__);
    return;
  }

  // get data process 
  int module_index = 1;
  err = gpio_maglock001_get_port_num(module_index, &port_num);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_maglock001_get_port_num failed", __LINE__,
             __func__);
    return;
  }

  bool *state = (bool *)malloc(port_num * sizeof(bool));

  for (int i = 0; i < port_num; i++) {
    err = gpio_maglock001_get_state(module_index, i + 1, &state[i]);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s mt_maglock_get_state module:%d index:%d failed",
               __LINE__, __func__, module_index, i + 1);
      return;
    }
  }

  // data struct
  group = mt_module_flow_new_struct_group(port_num);

  for (int i = 0; i < port_num; i++) {
    // state%(index)
    sprintf(key, "state%d", i + 1);
    group->value[count]->key = malloc(strlen(key) + 1);
    memcpy(group->value[count]->key, key, strlen(key) + 1);
    group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE;
    group->value[count++]->bool_value = state[i];
  }

  // send msg
  err = mt_module_flow_sent_msg(module_flow, group);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  // log
  ESP_LOGI(TAG, "%4d %s session:%s sent success", __LINE__, __func__,
           module_flow->session);
  for (int i = 0; i < port_num; i++) {
    ESP_LOGI(TAG, "%4d %s index:%d state:%d", __LINE__, __func__, i + 1,
             state[i]);
  }

EXIT:
  free(state);
  if (group != NULL)
    mt_module_flow_free_struct_group(group);

  return;
}

static void gpio_maglock001_module_flow_loop(mt_module_flow_t *module_flow) {
  esp_err_t err = ESP_OK;
  int module_index = 1;
  int count = 0;

  while (true) {
    bool has_change = false;

    err = gpio_maglock001_get_has_changed(module_index, &has_change);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s gpio_maglock001_get_has_changed failed", __LINE__,
               __func__);
      goto EXIT;
    }

    if (has_change == true) {
      gpio_maglock001_module_get_state_process(module_flow);
      count = 0;
    } else {
      if (count * 1000 > module_flow->push_frame_interval) {
        gpio_maglock001_module_get_state_process(module_flow);
        count = 0;
      }
    }

  EXIT:
    vTaskDelay(1000 / portTICK_RATE_MS);
    count++;
  }
}

void gpio_maglock001_module_flow_task(mt_module_flow_t *module_flow) {
  xTaskCreate((TaskFunction_t)gpio_maglock001_module_flow_loop, "MODULE_FLOW",
              8 * 1024, module_flow, 10, NULL);
}
