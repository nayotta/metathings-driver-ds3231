#include "modbus_power001_lora_flow.h"

#include "freertos/FreeRTOS.h"

#include "modbus_power001.h"
#include "modbus_power001_storage.h"
#include "mt_device_type.h"
#include "mt_module_flow.h"
#include "mt_module_lora.h"
#include "rs232_lora_ebyte.h"

// static define ==============================================================

static const char *TAG = "MODBUS_POWER001_LORA_FLOW";
static int FLOW_INTERVAL = 600; // 600s
static modbus_power001_config_t *CONFIG = NULL;
static int LORA_ID = 0;

// static func ================================================================

// no session here!!! must add before mqtt sent
static uint8_t *modbus_power001_pack_frame(modbus_power001_datas_t *datas,
                                           int *frame_size) {
  mt_module_flow_struct_group_t *group = NULL;
  int push_count = 0;
  int count = 0;
  char key[24] = "";
  uint8_t *frame_req_buf = NULL;

  for (int i = 0; i < datas->port_num; i++) {
    if (datas->datas[i] != NULL) {
      push_count++;
    }
  }

  int struct_size = 3 * push_count;
  group = mt_module_flow_new_struct_group(struct_size);

  for (int i = 1; i <= datas->port_num; i++) {
    if (datas->datas[i] != NULL) {

      // quality
      sprintf(key, "Q%d", i);
      group->value[count]->key = malloc(strlen(key) + 1);
      memcpy(group->value[count]->key, key, strlen(key) + 1);
      group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
      group->value[count++]->number_value = datas->datas[i]->quality;

      // voltage
      sprintf(key, "U%d", i);
      group->value[count]->key = malloc(strlen(key) + 1);
      memcpy(group->value[count]->key, key, strlen(key) + 1);
      group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
      group->value[count++]->number_value = datas->datas[i]->voltage;

      // CURRENT
      sprintf(key, "I%d", i);
      group->value[count]->key = malloc(strlen(key) + 1);
      memcpy(group->value[count]->key, key, strlen(key) + 1);
      group->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
      group->value[count++]->number_value = datas->datas[i]->current;
    }
  }

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

static void modbus_power001_push_frame() {
  esp_err_t err = ESP_OK;
  modbus_power001_datas_t *datas = NULL;
  int frame_size;
  uint8_t *frame_buf = NULL;
  rs232_lora_ebyte_data_t *ebyte_data = NULL;

  // get datas
  datas = modbus_power001_get_datas(CONFIG);
  if (datas == NULL) {
    ESP_LOGE(TAG, "%4d %s modbus_power001_get_datas failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // marshal datas
  frame_buf = modbus_power001_pack_frame(datas, &frame_size);
  if (frame_buf == NULL) {
    ESP_LOGE(TAG, "%4d %s modbus_power001_pack_frame failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // lora pack datas
  ebyte_data = rs232_lora_ebyte_new_data();
  ebyte_data->id = LORA_ID;
  ebyte_data->type = MT_DEVICE_TYPE_POWER001;
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
  modbus_power001_free_datas(datas);
  if (frame_buf != NULL)
    free(frame_buf);
  rs232_lora_ebyte_free_data(ebyte_data);
}

static void modbus_power001_lora_flow_loop() {
  while (true) {
    modbus_power001_push_frame();
    vTaskDelay(FLOW_INTERVAL * 1000 / portTICK_RATE_MS);
  }
}

// help func ==================================================================

esp_err_t modbus_power001_update_interval(int interval) {
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

esp_err_t modbus_power001_updata_config(modbus_power001_config_t *config) {
  if (config == NULL) {
    ESP_LOGE(TAG, "%4d %s config NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (config->power_num <= 0) {
    ESP_LOGE(TAG, "%4d %s config->power_num:%d error", __LINE__, __func__,
             config->power_num);
    return ESP_ERR_INVALID_ARG;
  }

  if (config->power_addr == NULL) {
    ESP_LOGE(TAG, "%4d %s config->power_addr  NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (CONFIG != NULL) {
    modbus_power001_storage_free_config(CONFIG);
  }

  CONFIG = config;

  return ESP_OK;
}

// global func ================================================================

esp_err_t modbus_power001_lora_flow_task() {
  esp_err_t err = ESP_OK;

  err = modbus_power001_storage_get_interval(&FLOW_INTERVAL);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_power001_storage_get_interval failed",
             __LINE__, __func__);
    return err;
  }

  CONFIG = modbus_power001_storage_get_config();
  if (CONFIG == NULL) {
    ESP_LOGE(TAG, "%4d %s modbus_power001_storage_get_config failed", __LINE__,
             __func__);
    return err;
  }

  err = mt_module_lora_get_client_id(&LORA_ID);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_lora_get_client_id failed", __LINE__,
             __func__);
    return err;
  }

  xTaskCreate((TaskFunction_t)modbus_power001_lora_flow_loop, "LORA_FLOW_TASK",
              8 * 1024, NULL, 10, NULL);

  return ESP_OK;
}
