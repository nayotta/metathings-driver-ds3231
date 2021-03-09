#include "modbus_gzpd800T.h"

#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "mt_mbfunc.h"
#include "mt_mbtask.h"
#include "mt_port.h"

// global define ==============================================================

static const char *TAG = "MODBUS_gzpd800T";
static int gzpd800T_port_num = 4;
#define GZPD800T_DEAULT_ADDR 01
#define GZPD800T_READ_DATA 03
#define GZPD800T_READ_WARN 01

#define GZPD800T_DATA_ADDR 0x0300
#define GZPD800T_ONE_DATA_SIZE 6
#define GZPD800T_WARN_SIZE 1

// help func ================================================================

gzpd800T_data_t *modbus_gzpd800T_new_data() {
  gzpd800T_data_t *data = malloc(sizeof(gzpd800T_data_t));

  data->port_num = gzpd800T_port_num;
  data->port_data = malloc(data->port_num * sizeof(gzpd800T_port_data_t *));
  for (int i = 0; i < data->port_num; i++) {
    data->port_data[i] = malloc(sizeof(gzpd800T_port_data_t));
    data->port_data[i]->amp = 0;
    data->port_data[i]->freq = 0;
    data->port_data[i]->power = 0;
    data->port_data[i]->warn = 0;
  }

  return data;
}

void modbus_gzpd800T_free_data(gzpd800T_data_t *data) {
  if (data == NULL)
    return;

  if (data->port_data != NULL) {
    for (int i = 0; i < data->port_num; i++) {
      if (data->port_data[i] != NULL)
        free(data->port_data[i]);
    }
    free(data->port_data);
  }

  free(data);
}

// global func ================================================================

gzpd800T_data_t *modbus_gzpd800T_get_data() {
  gzpd800T_data_t *data = modbus_gzpd800T_new_data();
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // cmd data
  err = modbus_sync_Cmd_03(GZPD800T_DEAULT_ADDR, GZPD800T_DATA_ADDR,
                           gzpd800T_port_num * GZPD800T_ONE_DATA_SIZE,
                           &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    goto EXIT;
  }

  // res check
  if (cmd_ret_payload.recvCmd != GZPD800T_READ_DATA) {
    ESP_LOGE(TAG, "%4d %s  get error ret cmd:%d", __LINE__, __func__,
             cmd_ret_payload.recvCmd);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  if (cmd_ret_payload.retLen !=
      2 * gzpd800T_port_num * GZPD800T_ONE_DATA_SIZE) {
    ESP_LOGE(TAG, "%4d %s get error ret size:%d", __LINE__, __func__,
             cmd_ret_payload.retLen);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // data parse
  for (int i = 0; i < gzpd800T_port_num; i++) {
    data->port_data[i]->amp =
        (uint32_t)cmd_ret_payload.retBuf[i * 2 * GZPD800T_ONE_DATA_SIZE + 0]
            << 24 |
        (uint32_t)cmd_ret_payload.retBuf[i * 2 * GZPD800T_ONE_DATA_SIZE + 1]
            << 16 |
        (uint32_t)cmd_ret_payload.retBuf[i * 2 * GZPD800T_ONE_DATA_SIZE + 2]
            << 8 |
        (uint32_t)cmd_ret_payload.retBuf[i * 2 * GZPD800T_ONE_DATA_SIZE + 3];
    data->port_data[i]->freq =
        (uint32_t)cmd_ret_payload.retBuf[i * 2 * GZPD800T_ONE_DATA_SIZE + 4]
            << 24 |
        (uint32_t)cmd_ret_payload.retBuf[i * 2 * GZPD800T_ONE_DATA_SIZE + 5]
            << 16 |
        (uint32_t)cmd_ret_payload.retBuf[i * 2 * GZPD800T_ONE_DATA_SIZE + 6]
            << 8 |
        (uint32_t)cmd_ret_payload.retBuf[i * 2 * GZPD800T_ONE_DATA_SIZE + 7];
    data->port_data[i]->power =
        (uint32_t)cmd_ret_payload.retBuf[i * 2 * GZPD800T_ONE_DATA_SIZE + 8]
            << 24 |
        (uint32_t)cmd_ret_payload.retBuf[i * 2 * GZPD800T_ONE_DATA_SIZE + 9]
            << 16 |
        (uint32_t)cmd_ret_payload.retBuf[i * 2 * GZPD800T_ONE_DATA_SIZE + 10]
            << 8 |
        (uint32_t)cmd_ret_payload.retBuf[i * 2 * GZPD800T_ONE_DATA_SIZE + 11];
  }

  // cmd warn
  err = modbus_sync_Cmd_01(GZPD800T_DEAULT_ADDR, 1,
                           gzpd800T_port_num * GZPD800T_WARN_SIZE,
                           &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    goto EXIT;
  }

  // res check
  if (cmd_ret_payload.recvCmd != GZPD800T_READ_WARN) {
    ESP_LOGE(TAG, "%4d %s  get error ret cmd:%d", __LINE__, __func__,
             cmd_ret_payload.recvCmd);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  if (cmd_ret_payload.retLen != 2 * gzpd800T_port_num * GZPD800T_WARN_SIZE) {
    ESP_LOGE(TAG, "%4d %s get error ret size:%d", __LINE__, __func__,
             cmd_ret_payload.retLen);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // parse warn
  for (int i = 0; i < gzpd800T_port_num; i++) {
    if (cmd_ret_payload.retBuf[i * 2 + 1] == 0)
      data->port_data[i]->warn = false;
    else
      data->port_data[i]->warn = true;
  }

  // log
  ESP_LOGI(TAG, "%4d %s port_num:%d", __LINE__, __func__, data->port_num);
  for (int i = 0; i < gzpd800T_port_num; i++) {
    ESP_LOGI(TAG, "%4d %s port:%2d amp:%08d freq:%08d power:%08d warn:%2d",
             __LINE__, __func__, i + 1, data->port_data[i]->amp,
             data->port_data[i]->freq, data->port_data[i]->power,
             data->port_data[i]->warn);
  }

EXIT:
  if (err != ESP_OK) {
    modbus_gzpd800T_free_data(data);
    data = NULL;
  }

  return data;
}

esp_err_t modbus_gzpd800T_init(uint8_t port, int tx_pin, int rx_pin, int en_pin,
                               int port_num) {
  eMBErrorCode emb_ret = 0;
  UCHAR RS485_PORT = 2;
  ULONG RS485_BAUD = 9600;
  eMBParity RS485_PARITY = MB_PAR_NONE;

  emb_ret =
      modbus_init(RS485_PORT, RS485_BAUD, RS485_PARITY, tx_pin, rx_pin, en_pin);
  if (emb_ret != 0) {
    ESP_LOGE(TAG, "%4d %s modbus_init failed", __LINE__, __func__);
    return emb_ret;
  }

  mt_vMBMaster_set_T35_interval(125);
  mt_modbus_task();

  gzpd800T_port_num = port_num;

  return ESP_OK;
}

cJSON *modbus_gzpd800T_get_flow_data() {
  esp_err_t err = ESP_OK;
  cJSON *json_data = cJSON_CreateObject();
  char key[24] = "";
  gzpd800T_data_t *data = NULL;

  data = modbus_gzpd800T_get_data();
  if (data == NULL) {
    ESP_LOGE(TAG, "%4d %s modbus_gzpd800T_get_data failed", __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  for (int i = 0; i < data->port_num; i++) {
    // amp
    sprintf(key, "amp%d", i + 1);
    cJSON_AddNumberToObject(json_data, key, data->port_data[i]->amp);

    // freq
    sprintf(key, "freq%d", i + 1);
    cJSON_AddNumberToObject(json_data, key, data->port_data[i]->freq);

    // power
    sprintf(key, "power%d", i + 1);
    cJSON_AddNumberToObject(json_data, key, data->port_data[i]->power);

    // warn
    sprintf(key, "warn%d", i + 1);
    cJSON_AddBoolToObject(json_data, key, data->port_data[i]->warn);
  }

EXIT:
  if (err != ESP_OK) {
    cJSON_Delete(json_data);
    json_data = NULL;
  }
  modbus_gzpd800T_free_data(data);
  return json_data;
}
