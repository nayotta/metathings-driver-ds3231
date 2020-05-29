
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "modbus_aew100.h"
#include "modbus_aew100_module_flow.h"
#include "modbus_aew100_module_mqtt.h"

#include "mt_module_flow.h"
#include "mt_module_http.h"
#include "mt_mqtt_lan.h"
#include "mt_mqtt_utils.h"

#include "google/protobuf/struct.pb-c.h"
#include "push_frame_to_flow.pb-c.h"

// global define ==============================================================

static const char *TAG = "MODULE_AEW100_FLOW";

// global func ================================================================

void module_aew100_get_current_process(mt_module_flow_t *module_flow) {
  esp_err_t err = ESP_OK;
  mt_module_flow_struct_group_t *group = NULL;
  MtAew100__Data data;
  int count = 0;

  // get current data
  err = mt_aew100_get_data2(module_flow->flow_index, &data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_aew100_get_data failed", __LINE__, __func__);
    goto EXIT;
  }

  // data struct
  int struct_size = 15;
  group = mt_module_flow_new_struct_group(struct_size);

  // currentA
  if (data.has_currenta) {
    mt_module_flow_set_number_value(group->value[count++], "currentA",
                                    data.currenta);
  }

  // currentB
  if (data.has_currentb) {
    mt_module_flow_set_number_value(group->value[count++], "currentB",
                                    data.currentb);
  }

  // currentC
  if (data.has_currentc) {
    mt_module_flow_set_number_value(group->value[count++], "currentC",
                                    data.currentc);
  }

  // votageA
  if (data.has_votagea) {
    mt_module_flow_set_number_value(group->value[count++], "votageA",
                                    data.votagea);
  }

  // votageB
  if (data.has_votageb) {
    mt_module_flow_set_number_value(group->value[count++], "votageB",
                                    data.votageb);
  }

  // votageC
  if (data.has_votagec) {
    mt_module_flow_set_number_value(group->value[count++], "votageC",
                                    data.votagec);
  }

  // activePowerA
  if (data.has_activepowera) {
    mt_module_flow_set_number_value(group->value[count++], "activePowerA",
                                    data.activepowera);
  }

  // activePowerB
  if (data.has_activepowerb) {
    mt_module_flow_set_number_value(group->value[count++], "activePowerB",
                                    data.activepowerb);
  }

  // activePowerC
  if (data.has_activepowerc) {
    mt_module_flow_set_number_value(group->value[count++], "activePowerC",
                                    data.activepowerc);
  }

  // powerFactorA
  if (data.has_powerfactora) {
    mt_module_flow_set_number_value(group->value[count++], "powerFactorA",
                                    data.powerfactora);
  }

  // powerFactorB
  if (data.has_powerfactorb) {
    mt_module_flow_set_number_value(group->value[count++], "powerFactorB",
                                    data.powerfactorb);
  }

  // powerFactorC
  if (data.has_powerfactorc) {
    mt_module_flow_set_number_value(group->value[count++], "powerFactorC",
                                    data.powerfactorc);
  }

  // tempA
  if (data.has_tempa) {
    mt_module_flow_set_number_value(group->value[count++], "tempA", data.tempa);
  }

  // tempB
  if (data.has_tempb) {
    mt_module_flow_set_number_value(group->value[count++], "tempB", data.tempb);
  }

  // tempC
  if (data.has_tempc) {
    mt_module_flow_set_number_value(group->value[count++], "tempC", data.tempc);
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
  for (int j = 0; j < group->size; j++) {
    ESP_LOGI(TAG, "%4d %s key:%s value:%f", __LINE__, __func__,
             group->value[j]->key, group->value[j]->number_value);
  }

EXIT:
  if (group != NULL)
    mt_module_flow_free_struct_group(group);

  return;
}

static void module_aew100_flow_loop(mt_module_flow_t *module_flow) {
  while (true) {
    module_aew100_get_current_process(module_flow);
    vTaskDelay(module_flow->push_frame_interval / portTICK_RATE_MS);
  }
}

void modbus_aew100_module_flow_task(mt_module_flow_t *module_flow) {
  xTaskCreate((TaskFunction_t)module_aew100_flow_loop, "MODULE_FLOW", 8 * 1024,
              module_flow, 10, NULL);
}
