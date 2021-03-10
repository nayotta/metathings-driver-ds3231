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

#include "mt_nvs_storage.h"
#include "mt_utils_string.h"

#include "modbus_ktc_n01.h"

// static define ==============================================================

static const char *TAG = "MODBUS_KTC_N01";

static int Device_num = 1;

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

esp_err_t modbus_ktc_n01_init(uint8_t port, int tx_pin, int rx_pin, int en_pin,
                              int device_num) {
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

  Device_num = device_num;

  return ESP_OK;
}

esp_err_t modbus_ktc_n01_with_config_init(uint8_t port, int tx_pin, int rx_pin,
                                          int en_pin) {
  int32_t device_num = 0;
  esp_err_t err = ESP_OK;

  if (mt_nvs_read_int32_config("device_num", &device_num) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_read_int32_config device_num failed", __LINE__,
             __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  err = modbus_ktc_n01_init(port, tx_pin, rx_pin, en_pin, device_num);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_ktc_n01_init failed", __LINE__, __func__);
    return err;
  }

  return ESP_OK;
}

// json api func ==============================================================

cJSON *modbus_ktc_n01_get_flow_data() {
  esp_err_t err = ESP_OK;
  cJSON *json_data = cJSON_CreateObject();
  char key[24] = "";
  bool exist = false;
  double temp = 0;
  double hum = 0;
  double current = 0;

  for (int i = 0; i < Device_num; i++) {
    err = modbus_ktc_n01_get_temp_hum(i + 1, &temp, &hum);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s modbus_ktc_n01_get_temp_hum failed", __LINE__,
               __func__);
    } else {
      exist = true;
    }

    err = modbus_ktc_n01_get_current(i + 1, &current);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s modbus_ktc_n01_get_current failed", __LINE__,
               __func__);
    } else {
      exist = true;
    }

    // temp
    sprintf(key, "temp%d", i + 1);
    cJSON_AddNumberToObject(json_data, key, temp);

    // hum
    sprintf(key, "hum%d", i + 1);
    cJSON_AddNumberToObject(json_data, key, hum);

    // current
    sprintf(key, "current%d", i + 1);
    cJSON_AddNumberToObject(json_data, key, current);
  }

  if (exist == false) {
    cJSON_Delete(json_data);
    json_data = NULL;
  }
  return json_data;
}

cJSON *modbus_ktc_n01_json_set_cmd(cJSON *paras) {
  esp_err_t err = ESP_OK;
  cJSON *res = cJSON_CreateObject();
  cJSON *item = NULL;
  int32_t port = 0;
  bool port_exsit = false;
  int32_t cmd = 0;
  bool cmd_exsit = false;

  // parse paras
  for (int i = 0; i < cJSON_GetArraySize(paras); i++) {
    item = cJSON_GetArrayItem(paras, i);
    if (item->type == cJSON_Number) {
      if (strcmp(item->string, "port") == 0) {
        port = (int)item->valuedouble;
        port_exsit = true;
      }

      if (strcmp(item->string, "cmd") == 0) {
        cmd = (int)item->valuedouble;
        cmd_exsit = true;
      }
    }
  }

  // parse check
  if (port_exsit == false || cmd_exsit == false) {
    ESP_LOGE(TAG, "%4d %s port_exsit:%d cmd_exsit:%d arg check error", __LINE__,
             __func__, port_exsit, cmd_exsit);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // cmd process
  err = modbus_ktc_n01_set_cmd(port, cmd);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_ktc_n01_set_cmd failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  cJSON_AddNumberToObject(res, "code", err);
  cJSON_AddStringToObject(res, "content", esp_err_to_name(err));

  return res;
}
