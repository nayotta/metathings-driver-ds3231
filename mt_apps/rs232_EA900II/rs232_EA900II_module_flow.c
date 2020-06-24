#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "mt_module_flow.h"
#include "mt_module_http.h"
#include "mt_mqtt_lan.h"
#include "mt_mqtt_utils.h"
#include "mt_nvs_storage.h"
#include "rs232_EA900II.h"
#include "rs232_EA900II_module_flow.h"
#include "rs232_EA900II_module_mqtt.h"

#include "google/protobuf/struct.pb-c.h"

#include "mt_proto_EA900II.pb-c.h"

// global define ==============================================================

static const char *TAG = "MODULE_FLOW";

static mt_module_flow_t *MODULE_FLOW = NULL;

// static  func ================================================================

static void module_get_datas_process() {
  esp_err_t err = ESP_OK;
  mt_module_flow_struct_group_t *group = NULL;
  rs232_EA900II_status_t *status = NULL;
  int count = 0;
  char key[24] = "";

  // get data process
  status = rs232_EA900II_get_status();
  if (status == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_EA900II_get_status failed", __LINE__, __func__);
    return;
  }

  // data struct
  int struct_size = 14;
  group = mt_module_flow_new_struct_group(struct_size);

  // vIn
  sprintf(key, "vIn");
  group->value[count]->key = malloc(strlen(key) + 1);
  memcpy(group->value[count]->key, key, strlen(key) + 1);
  group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  group->value[count++]->number_value = status->vIn;

  // vOut
  sprintf(key, "vOut");
  group->value[count]->key = malloc(strlen(key) + 1);
  memcpy(group->value[count]->key, key, strlen(key) + 1);
  group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  group->value[count++]->number_value = status->vOut;

  // load
  sprintf(key, "load");
  group->value[count]->key = malloc(strlen(key) + 1);
  memcpy(group->value[count]->key, key, strlen(key) + 1);
  group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  group->value[count++]->number_value = status->load;

  // freqIn
  sprintf(key, "freqIn");
  group->value[count]->key = malloc(strlen(key) + 1);
  memcpy(group->value[count]->key, key, strlen(key) + 1);
  group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  group->value[count++]->number_value = status->freqIn;

  // vBat
  sprintf(key, "vBat");
  group->value[count]->key = malloc(strlen(key) + 1);
  memcpy(group->value[count]->key, key, strlen(key) + 1);
  group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  group->value[count++]->number_value = status->vBat;

  // temp
  sprintf(key, "temp");
  group->value[count]->key = malloc(strlen(key) + 1);
  memcpy(group->value[count]->key, key, strlen(key) + 1);
  group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  group->value[count++]->number_value = status->temp;

  // w7
  sprintf(key, "w7");
  group->value[count]->key = malloc(strlen(key) + 1);
  memcpy(group->value[count]->key, key, strlen(key) + 1);
  group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  group->value[count++]->number_value = status->w7;

  // w6
  sprintf(key, "w6");
  group->value[count]->key = malloc(strlen(key) + 1);
  memcpy(group->value[count]->key, key, strlen(key) + 1);
  group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  group->value[count++]->number_value = status->w6;

  // w5
  sprintf(key, "w5");
  group->value[count]->key = malloc(strlen(key) + 1);
  memcpy(group->value[count]->key, key, strlen(key) + 1);
  group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  group->value[count++]->number_value = status->w5;

  // w4
  sprintf(key, "w4");
  group->value[count]->key = malloc(strlen(key) + 1);
  memcpy(group->value[count]->key, key, strlen(key) + 1);
  group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  group->value[count++]->number_value = status->w4;

  // w3
  sprintf(key, "w3");
  group->value[count]->key = malloc(strlen(key) + 1);
  memcpy(group->value[count]->key, key, strlen(key) + 1);
  group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  group->value[count++]->number_value = status->w3;

  // w2
  sprintf(key, "w2");
  group->value[count]->key = malloc(strlen(key) + 1);
  memcpy(group->value[count]->key, key, strlen(key) + 1);
  group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  group->value[count++]->number_value = status->w2;

  // w1
  sprintf(key, "w1");
  group->value[count]->key = malloc(strlen(key) + 1);
  memcpy(group->value[count]->key, key, strlen(key) + 1);
  group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  group->value[count++]->number_value = status->w1;

  // w0
  sprintf(key, "w0");
  group->value[count]->key = malloc(strlen(key) + 1);
  memcpy(group->value[count]->key, key, strlen(key) + 1);
  group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  group->value[count++]->number_value = status->w0;

  // send msg
  err = mt_module_flow_sent_msg(MODULE_FLOW, group);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  // log
  ESP_LOGI(TAG,
           "%4d %s session:%s sent success \n"
           "vIn:%3.2f vOut:%3.2f load:%3.2f freqIn:%3.2f "
           "vBat:%3.2f temp:%3.2f\n"
           "w0:%1d w1:%1d w2:%1d w3:%1d w4:%1d w5:%1d w6:%1d w7:%1d\n",
           __LINE__, __func__, MODULE_FLOW->session, status->vIn, status->vOut,
           status->load, status->freqIn, status->vBat, status->temp, status->w0,
           status->w1, status->w2, status->w3, status->w4, status->w5,
           status->w6, status->w7);

EXIT:
  if (status != NULL)
    free(status);

  if (group != NULL)
    mt_module_flow_free_struct_group(group);

  return;
}

static void module_flow_loop(mt_module_flow_t *module_flow) {
  while (true) {
    module_get_datas_process();
    vTaskDelay(MODULE_FLOW->push_frame_interval / portTICK_RATE_MS);
  }
}

// global func ================================================================

void rs232_EA900II_module_flow_task(mt_module_flow_t *module_flow) {
  MODULE_FLOW = module_flow;
  xTaskCreate((TaskFunction_t)module_flow_loop, "EA900II_FLOW", 8 * 1024,
              module_flow, 10, NULL);
}
