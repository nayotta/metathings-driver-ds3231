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
#include "mt_nvs_storage.h"
#include "mt_port.h"

#include "modbus_bits001.h"

// static define ==============================================================

static const char *TAG = "MODBUS_BITS001";

static enum MODBUS_BITS001_TYPE Device_type = MODBUS_BITS001_TYPE_TEMP_HUM;
static int Device_num = 1;

#define BITS001_READ 03

#define BITS001_CMD_TEMP_ADDR 01
#define BITS001_CMD_HUM_ADDR 02
#define BITS001_CMD_FROG_ADDR 11

// static func ================================================================

esp_err_t modbus_bits001_get_temp_hum(int addr, double *temp, double *hum) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload = {"", 0, 0};

  err = modbus_sync_Cmd_03(addr, BITS001_CMD_TEMP_ADDR, 2, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    return err;
  }

  if (cmd_ret_payload.recvCmd != BITS001_READ) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
             addr, cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != 4) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
             addr, cmd_ret_payload.retLen);
    return ESP_ERR_INVALID_RESPONSE;
  }

  *temp = (((uint32_t)(cmd_ret_payload.retBuf[0]) << 8) +
           cmd_ret_payload.retBuf[1] - 2000) /
          100.0;
  *hum = (((uint32_t)(cmd_ret_payload.retBuf[2]) << 8) +
          cmd_ret_payload.retBuf[3]) /
         100.0;

  return ESP_OK;
}

esp_err_t modbus_bits001_get_frog(int addr, double *frog) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  err = modbus_sync_Cmd_03(addr, BITS001_CMD_FROG_ADDR, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    return err;
  }

  if (cmd_ret_payload.recvCmd != BITS001_READ) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
             addr, cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != 2) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
             addr, cmd_ret_payload.retLen);
    return ESP_ERR_INVALID_RESPONSE;
  }

  *frog = (((uint32_t)(cmd_ret_payload.retBuf[0]) << 8) +
           cmd_ret_payload.retBuf[1]);

  return ESP_OK;
}

esp_err_t modbus_bits001_get_data(int addr, double *temp, double *hum,
                                  double *frog) {
  esp_err_t err = ESP_OK;

  err = modbus_bits001_get_temp_hum(addr, temp, hum);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_get_temp_hum failed,code=%d", __LINE__,
             __func__, err);
    return err;
  }

  err = modbus_bits001_get_frog(addr, frog);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_get_frog failed,code=%d", __LINE__,
             __func__, err);
    return err;
  }

  return ESP_OK;
}

esp_err_t modbus_bits001_init(uint8_t port, int tx_pin, int rx_pin, int en_pin,
                              int device_num, enum MODBUS_BITS001_TYPE type) {
  eMBErrorCode emb_ret = 0;
  ULONG RS485_BAUD = 9600;
  eMBParity RS485_PARITY = MB_PAR_NONE;

  emb_ret = modbus_init(port, RS485_BAUD, RS485_PARITY, tx_pin, rx_pin, en_pin);
  if (emb_ret != 0) {
    ESP_LOGE(TAG, "%4d %s modbus_init failed", __LINE__, __func__);
    return emb_ret;
  }

  mt_vMBMaster_set_T35_interval(500);
  mt_modbus_task();

  Device_num = device_num;
  Device_type = type;

  return ESP_OK;
}

esp_err_t modbus_bits001_with_config_init(uint8_t port, int tx_pin, int rx_pin,
                                          int en_pin) {
  esp_err_t err = ESP_OK;
  int32_t device_num = 0;
  char *device_type = NULL;
  size_t device_type_len = 0;

  if (mt_nvs_read_int32_config("device_num", &device_num) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_read_int32_config device_num failed", __LINE__,
             __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  device_type = mt_nvs_read_string_config("device_type", &device_type_len);
  if (device_type == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_read_string_config device_type failed",
             __LINE__, __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (strcmp(device_type, "t_h") == 0)
    Device_type = MODBUS_BITS001_TYPE_TEMP_HUM;
  else if (strcmp(device_type, "f") == 0)
    Device_type = MODBUS_BITS001_TYPE_FOG;
  else if (strcmp(device_type, "t_h_f") == 0)
    Device_type = MODBUS_BITS001_TYPE_TEMP_HUM_FOG;
  else {
    ESP_LOGE(TAG, "%4d %s unexpect device_type:%s failed", __LINE__, __func__,
             device_type);
    return ESP_ERR_INVALID_ARG;
  }

  err = modbus_bits001_init(port, tx_pin, rx_pin, en_pin, device_num,
                            Device_type);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_init failed", __LINE__, __func__);
    return err;
  }

  return ESP_OK;
}

cJSON *modbus_bits001_get_flow_data() {
  esp_err_t err = ESP_OK;
  cJSON *json_data = cJSON_CreateObject();
  char key[24] = "";
  bool exist = false;
  double temp = 0;
  double hum = 0;
  double fog = 0;

  for (int i = 0; i < Device_num; i++) {
    switch (Device_type) {
    case MODBUS_BITS001_TYPE_TEMP_HUM:
      err = modbus_bits001_get_temp_hum(i + 1, &temp, &hum);
      if (err != ESP_OK) {
        ESP_LOGE(TAG, "%4d %s modbus_bits001_get_temp_hum failed", __LINE__,
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

      break;
    case MODBUS_BITS001_TYPE_FOG:
      err = modbus_bits001_get_frog(i + 1, &fog);
      if (err != ESP_OK) {
        ESP_LOGE(TAG, "%4d %s modbus_bits001_get_fog failed", __LINE__,
                 __func__);
      } else {
        exist = true;
      }

      // fog
      sprintf(key, "frog%d", i + 1);
      cJSON_AddNumberToObject(json_data, key, fog);

      break;
    case MODBUS_BITS001_TYPE_TEMP_HUM_FOG:
      err = modbus_bits001_get_data(i + 1, &temp, &hum, &fog);
      if (err != ESP_OK) {
        ESP_LOGE(TAG, "%4d %s modbus_bits001_get_temp_hum failed", __LINE__,
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

      // fog
      sprintf(key, "frog%d", i + 1);
      cJSON_AddNumberToObject(json_data, key, fog);

      break;
    default:
      ESP_LOGE(TAG, "%4d %s unexcept type:%d", __LINE__, __func__, Device_type);
    }
  }

  if (exist == false) {
    cJSON_Delete(json_data);
    json_data = NULL;
  }
  return json_data;
}
