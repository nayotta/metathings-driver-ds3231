#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "rs232_lora_jxyl001.h"
#include "rs232_lora_jxyl001_module_flow.h"

#include "mt_module_flow.h"
#include "mt_module_http.h"
#include "mt_mqtt_utils.h"

#include "google/protobuf/struct.pb-c.h"
#include "push_frame_to_flow.pb-c.h"

// global define ==============================================================

static const char *TAG = "RS232_JXYL001_FLOW";

// static func ================================================================

static void module_get_data_process(module_jxyl001_flow_t *jxyl001_flow) {
  esp_err_t err = ESP_OK;
  mt_module_flow_struct_group_t *group = NULL;
  mt_module_flow_t *module_flow = jxyl001_flow->module_flow;
  int count = 0;

  // data struct
  int struct_size =
      2 * jxyl001_flow->devs->fog_num + 3 * jxyl001_flow->devs->temp_num;
  group = mt_module_flow_new_struct_group(struct_size);

  // fog struct
  for (int i = 0; i < jxyl001_flow->devs->fog_num; i++) {
    if ((esp_timer_get_time() / 1000000 -
             jxyl001_flow->devs->fogs[i]->update_time <
         jxyl001_flow->module_flow->push_frame_interval / 1000) &
        (jxyl001_flow->devs->fogs[i]->update_time != 0)) {
      char key[24] = "";

      // frog
      sprintf(key, "frog%d", i + 1);
      mt_module_flow_set_number_value(group->value[count++], key,
                                      jxyl001_flow->devs->fogs[i]->fog * 1000);

      // bat frog
      sprintf(key, "batfog%d", i + 1);
      mt_module_flow_set_number_value(group->value[count++], key,
                                      jxyl001_flow->devs->fogs[i]->votage);
    }
  }

  // temp struct
  for (int i = 0; i < jxyl001_flow->devs->temp_num; i++) {
    if ((esp_timer_get_time() / 1000000 -
             jxyl001_flow->devs->temps[i]->update_time <
         jxyl001_flow->module_flow->push_frame_interval / 1000) &
        (jxyl001_flow->devs->temps[i]->update_time != 0)) {
      char key[24] = "";

      // temp
      sprintf(key, "temp%d", i + 1);
      mt_module_flow_set_number_value(group->value[count++], key,
                                      jxyl001_flow->devs->temps[i]->temp);

      // hum
      sprintf(key, "hum%d", i + 1);
      mt_module_flow_set_number_value(group->value[count++], key,
                                      jxyl001_flow->devs->temps[i]->hum);

      // bat temp
      sprintf(key, "battemp%d", i + 1);
      mt_module_flow_set_number_value(group->value[count++], key,
                                      jxyl001_flow->devs->temps[i]->votage);
    }
  }

  // send msg
  if (count == 0) {
    ESP_LOGI(TAG, "%4d %s nothing need push", __LINE__, __func__);
    goto EXIT;
  }
  err = mt_module_flow_sent_msg(module_flow, group);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  // log
  ESP_LOGI(TAG, "%4d %s session:%s sent success", __LINE__, __func__,
           module_flow->session);
  for (int j = 0; j < group->size; j++) {
    if (group->value[j]->key != NULL) {
      ESP_LOGI(TAG, "%4d %s key:%s value:%f", __LINE__, __func__,
               group->value[j]->key, group->value[j]->number_value);
    }
  }

EXIT:
  if (group != NULL)
    mt_module_flow_free_struct_group(group);

  return;
}

static void module_flow_loop(module_jxyl001_flow_t *jxyl001_flow) {
  while (true) {
    module_get_data_process(jxyl001_flow);
    vTaskDelay(jxyl001_flow->module_flow->push_frame_interval /
               portTICK_RATE_MS);
  }
}

// global func ================================================================

void rs232_lora_jxyl001_module_flow_task(mt_module_flow_t *module_flow) {
  module_jxyl001_flow_t *jxyl001_flow = malloc(sizeof(module_jxyl001_flow_t));

  jxyl001_flow->module_flow = module_flow;
  jxyl001_flow->devs = rs232_lora_jxyl001_get_devs();

  xTaskCreate((TaskFunction_t)module_flow_loop, "MODULE_FLOW", 8 * 1024,
              jxyl001_flow, 10, NULL);
}
