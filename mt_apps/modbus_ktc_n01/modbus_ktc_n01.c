#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "mt_mbfunc.h"
#include "mt_mbport.h"
#include "mt_mbtask.h"
#include "mt_port.h"
#include "mt_utils_string.h"

#include "mt_proto_ktc_n01.pb-c.h"

#include "modbus_ktc_n01.h"

// static define ==============================================================

static const char *TAG = "MODBUS_KTC_N01";

#define KTC_N01_DEFAULT_PORT 0x01

#define KTC_N01_READ 03
#define KTC_N01_WRITE 06

#define KTC_N01_CMD_TEMP_HUM 0x00
#define KTC_N01_CMD_CURRENT 0x71
#define KTC_N01_CMD_SET 0xB9

// global func ================================================================

esp_err_t modbus_ktc_n01_get_temp_hum(int addr, double *temp, double *hum) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload = {"", 0, 0};

  err = modbus_sync_Cmd_03(addr, KTC_N01_CMD_TEMP_HUM, 2, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    return err;
  }

  if (cmd_ret_payload.recvCmd != KTC_N01_READ) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
             addr, cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != 4) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
             addr, cmd_ret_payload.retLen);
    return ESP_ERR_INVALID_RESPONSE;
  }

  *hum = (((uint32_t)(cmd_ret_payload.retBuf[0]) << 8) +
           cmd_ret_payload.retBuf[1]) /
          10.0;
  *temp = (((uint32_t)(cmd_ret_payload.retBuf[2]) << 8) +
          cmd_ret_payload.retBuf[3]) /
         10.0;

  return ESP_OK;
}

esp_err_t modbus_ktc_n01_get_current(int addr, double *current) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload = {"", 0, 0};

  err = modbus_sync_Cmd_03(addr, KTC_N01_CMD_CURRENT, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    return err;
  }

  if (cmd_ret_payload.recvCmd != KTC_N01_READ) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
             addr, cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != 2) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
             addr, cmd_ret_payload.retLen);
    return ESP_ERR_INVALID_RESPONSE;
  }

  *current = (((uint32_t)(cmd_ret_payload.retBuf[0]) << 8) +
              cmd_ret_payload.retBuf[1]) /
             10.0;

  return ESP_OK;
}

esp_err_t modbus_ktc_n01_set_cmd(int addr, int cmd) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload = {"", 0, 0};

  int target = 0;
  switch (cmd) {
  case 101:
    target = KTC_N01_CMD_SET;
    break;
  case 102:
    target = KTC_N01_CMD_SET + 1;
    break;
  case 103:
    target = KTC_N01_CMD_SET + 2;
    break;
  default:
    target = KTC_N01_CMD_SET + 2 + cmd;
    break;
  }

  err = modbus_sync_Cmd_06(addr, target, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    return err;
  }

  if (cmd_ret_payload.recvCmd != KTC_N01_WRITE) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
             addr, cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != 2) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
             addr, cmd_ret_payload.retLen);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

mt_module_flow_struct_group_t *modbus_ktc_n01_get_flow_data() {
  esp_err_t err = ESP_OK;
  double temp = 0;
  double hum = 0;
  double current = 0;

  err = modbus_ktc_n01_get_temp_hum(KTC_N01_DEFAULT_PORT, &temp, &hum);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_ktc_n01_get_temp_hum failed", __LINE__,
             __func__);
    return NULL;
  }

  err = modbus_ktc_n01_get_current(KTC_N01_DEFAULT_PORT, &current);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_ktc_n01_get_current failed", __LINE__,
             __func__);
    return NULL;
  }

  int flow_size = 4;
  int count = 0;
  mt_module_flow_struct_group_t *data_out =
      mt_module_flow_new_struct_group(flow_size);

  // group->port
  data_out->value[count]->key = mt_utils_string_copy("port");
  data_out->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  data_out->value[count++]->number_value = KTC_N01_DEFAULT_PORT;

  // group->temp
  data_out->value[count]->key = mt_utils_string_copy("temp");
  data_out->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  data_out->value[count++]->number_value = temp;

  // group->hum
  data_out->value[count]->key = mt_utils_string_copy("hum");
  data_out->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  data_out->value[count++]->number_value = hum;

  // group->current
  data_out->value[count]->key = mt_utils_string_copy("current");
  data_out->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  data_out->value[count++]->number_value = current;

  ESP_LOGI(TAG, "%4d %s port:%d temp:%f hum:%f current:%f", __LINE__, __func__,
           KTC_N01_DEFAULT_PORT, temp, hum, current);

  return data_out;
}

esp_err_t modbus_ktc_n01_init(uint8_t port, int tx_pin, int rx_pin,
                              int en_pin) {
  eMBErrorCode emb_ret = 0;
  ULONG RS485_BAUD = 4800;
  eMBParity RS485_PARITY = MB_PAR_NONE;

  emb_ret = modbus_init(port, RS485_BAUD, RS485_PARITY, tx_pin, rx_pin, en_pin);
  if (emb_ret != 0) {
    ESP_LOGE(TAG, "%4d %s modbus_init failed", __LINE__, __func__);
    return emb_ret;
  }

  mt_vMBMaster_set_T35_interval(250);
  mt_modbus_task();

  return ESP_OK;
}
