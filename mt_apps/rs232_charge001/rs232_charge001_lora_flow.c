#include "rs232_charge001_lora_flow.h"

#include "freertos/FreeRTOS.h"

#include "mt_device_type.h"
#include "mt_module_flow.h"
#include "mt_module_lora.h"
#include "mt_nvs_config.h"
#include "rs232_lora_ebyte.h"

// static define ==============================================================

static const char *TAG = "RS232_CHARGE001_LORA_FLOW";
static int FLOW_INTERVAL = 600; // 600s
static int FLOW_INDEX = 1;      // module 1
static int LORA_ID = 0;

// static func ================================================================

// no session here!!! must add before mqtt sent
static uint8_t *rs232_charge001_pack_frame(rs232_charge001_states2_t *datas,
                                           int *frame_size) {
  mt_module_flow_struct_group_t *group = NULL;
  int push_count = 0;
  int count = 0;
  char key[24] = "";
  uint8_t *frame_req_buf = NULL;

  for (int i = 0; i < datas->num; i++) {
    if (datas->states[i] != NULL) {
      push_count++;
    }
  }

  int struct_size = 2 * push_count + 2;
  group = mt_module_flow_new_struct_group(struct_size);

  for (int i = 1; i <= datas->num; i++) {
    if (datas->states[i - 1] != NULL) {

      // state
      sprintf(key, "s%d", i);
      group->value[count]->key = malloc(strlen(key) + 1);
      memcpy(group->value[count]->key, key, strlen(key) + 1);
      group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
      group->value[count++]->number_value = datas->states[i - 1]->state;

      // timeleft
      sprintf(key, "t%d", i);
      group->value[count]->key = malloc(strlen(key) + 1);
      memcpy(group->value[count]->key, key, strlen(key) + 1);
      group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
      group->value[count++]->number_value = datas->states[i - 1]->lefttime;

      ESP_LOGI(TAG, "%4d %s index:%d state:%2d lefttime:%4d", __LINE__,
               __func__, i, datas->states[i - 1]->state,
               datas->states[i - 1]->lefttime);
    }
  }

  // set type
  sprintf(key, "type");
  group->value[count]->key = malloc(strlen(key) + 1);
  memcpy(group->value[count]->key, key, strlen(key) + 1);
  group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  group->value[count++]->number_value = MT_DEVICE_TYPE_CHARGE001;

  // set index
  sprintf(key, "index");
  group->value[count]->key = malloc(strlen(key) + 1);
  memcpy(group->value[count]->key, key, strlen(key) + 1);
  group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  group->value[count++]->number_value = 1;

  frame_req_buf = mt_module_flow_pack_frame(group, NULL, frame_size);
  if (frame_req_buf == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_pack_frame error", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  if (group != NULL)
    mt_module_flow_free_struct_group(group);
  return frame_req_buf;
}

static void rs232_charge001_push_frame() {
  esp_err_t err = ESP_OK;
  rs232_charge001_states2_t *datas = NULL;
  int frame_size;
  uint8_t *frame_buf = NULL;
  rs232_lora_ebyte_data_t *ebyte_data = NULL;

  // get datas
  datas = rs232_charge001_get_states2();
  if (datas == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_get_states2 failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // marshal datas
  frame_buf = rs232_charge001_pack_frame(datas, &frame_size);
  if (frame_buf == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_pack_frame failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // lora pack datas
  ebyte_data = rs232_lora_ebyte_new_data();
  ebyte_data->id = LORA_ID;
  ebyte_data->type = MT_DEVICE_TYPE_CHARGE001;
  ebyte_data->cmd = RS232_LORA_EBYTE_CMD_TYPE_FLOW;
  ebyte_data->len = frame_size;
  ebyte_data->data = malloc(frame_size);
  memcpy(ebyte_data->data, frame_buf, frame_size);

  // lora sent data
  err = rs232_lora_ebyte_sent(ebyte_data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_sent failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  rs232_charge001_free_states2(datas);
  if (frame_buf != NULL)
    free(frame_buf);
  rs232_lora_ebyte_free_data(ebyte_data);
}

static void rs232_charge001_lora_flow_loop() {
  while (true) {
    rs232_charge001_push_frame();
    vTaskDelay(FLOW_INTERVAL * 1000 / portTICK_RATE_MS);
  }
}

// help func ==================================================================

esp_err_t rs232_charge001_update_interval(int interval) {
  int old_interval = FLOW_INTERVAL;
  if (interval <= 0) {
    ESP_LOGE(TAG, "%4d %s interval:%d error", __LINE__, __func__, interval);
    return ESP_ERR_INVALID_ARG;
  }

  FLOW_INTERVAL = interval;
  ESP_LOGI(TAG, "%4d %s interval update from %d to %d", __LINE__, __func__,
           old_interval, FLOW_INTERVAL);

  return ESP_OK;
}

// global func ================================================================

esp_err_t rs232_charge001_lora_flow_task() {
  esp_err_t err = ESP_OK;

  err = mt_nvs_config_get_flow_interval(FLOW_INDEX, &FLOW_INTERVAL);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_get_flow_interval failed", __LINE__,
             __func__);
    return err;
  }

  err = mt_module_lora_get_client_id(&LORA_ID);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_lora_get_client_id failed", __LINE__,
             __func__);
    return err;
  }

  xTaskCreate((TaskFunction_t)rs232_charge001_lora_flow_loop, "LORA_FLOW_TASK",
              8 * 1024, NULL, 10, NULL);

  return ESP_OK;
}
