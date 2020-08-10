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

#include "modbus_bits001.h"

// static define ==============================================================

static const char *TAG = "MODBUS_BITS001";

#define BITS001_READ 03

#define BITS001_CMD_TEMP_ADDR 01
#define BITS001_CMD_HUM_ADDR 02
#define BITS001_CMD_FROG_ADDR 11

// static func ================================================================

esp_err_t modbus_bits001_get_temp(int addr, double *temp) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload = {"", 0, 0};

  err = modbus_sync_Cmd_03(addr, BITS001_CMD_TEMP_ADDR, 1, &cmd_ret_payload);
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

  *temp = (((uint32_t)(cmd_ret_payload.retBuf[0]) << 8) +
           cmd_ret_payload.retBuf[1] - 2000) /
          100.0;

  return ESP_OK;
}

esp_err_t modbus_bits001_get_hum(int addr, double *hum) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  err = modbus_sync_Cmd_03(addr, BITS001_CMD_HUM_ADDR, 1, &cmd_ret_payload);
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

  *hum = (((uint32_t)(cmd_ret_payload.retBuf[0]) << 8) +
          cmd_ret_payload.retBuf[1]) /
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

  err = modbus_bits001_get_temp(addr, temp);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_get_temp failed,code=%d", __LINE__,
             __func__, err);
    return err;
  }

  err = modbus_bits001_get_hum(addr, hum);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_get_hum failed,code=%d", __LINE__,
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

esp_err_t modbus_bits001_init(uint8_t port, int tx_pin, int rx_pin,
                              int en_pin) {
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

  return ESP_OK;
}
