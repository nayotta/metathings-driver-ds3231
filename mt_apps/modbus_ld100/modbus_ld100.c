#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "mt_mbfunc.h"
#include "mt_mbport.h"
#include "mt_port.h"

#include "mt_utils_string.h"

#include "modbus_ld100.h"
#include "mt_mbtask.h"

// global define ==============================================================

static const char *TAG = "MT_MODBUS_LD100";

#define LD100_READ 04
#define LD100_WRITE 06

#define LD100_CMD_DEV_ADDR 00
#define LD100_CMD_DEV_STATE 01
#define LD100_CMD_DEV_RELAY 03

// global func ================================================================

esp_err_t modbus_ld100_get_addr(int addr, bool *state) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  err = modbus_sync_Cmd_04(addr, LD100_CMD_DEV_ADDR, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    return err;
  }

  if (cmd_ret_payload.recvCmd != LD100_READ) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
             addr, cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != 2) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
             addr, cmd_ret_payload.retLen);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retBuf[0] != 0) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret buf 0:%d", __LINE__, __func__,
             addr, cmd_ret_payload.retBuf[0]);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retBuf[1] != addr) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret buf 1:%d", __LINE__, __func__,
             addr, cmd_ret_payload.retBuf[1]);
    return ESP_ERR_INVALID_RESPONSE;

    *state = true;
  }

  // ESP_LOGI(TAG, "%4d %s addr:%d state:%d", __LINE__, __func__, addr, *state);

  return ESP_OK;
}

esp_err_t modbus_ld100_set_addr(int addr, int new_addr) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  err =
      modbus_sync_Cmd_06(addr, LD100_CMD_DEV_ADDR, new_addr, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    return err;
  }

  if (cmd_ret_payload.recvCmd != LD100_WRITE) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
             addr, cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != 2) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
             addr, cmd_ret_payload.retLen);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retBuf[0] != 0) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret buf 0:%d", __LINE__, __func__,
             addr, cmd_ret_payload.retBuf[0]);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retBuf[1] != new_addr) {
    ESP_LOGE(TAG, "%4d %s addr:%d new_addr:%d error ret buf 1:%d", __LINE__,
             __func__, addr, new_addr, cmd_ret_payload.retBuf[1]);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // ESP_LOGI(TAG, "%4d %s addr:%d new_addr:%d success", __LINE__, __func__,
  // addr, new_addr);

  return ESP_OK;
}

esp_err_t modbus_ld100_get_state(int addr, bool *state) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  err = modbus_sync_Cmd_04(addr, LD100_CMD_DEV_STATE, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    return err;
  }

  if (cmd_ret_payload.recvCmd != LD100_READ) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
             addr, cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != 2) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
             addr, cmd_ret_payload.retLen);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retBuf[0] != 0) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret buf 0:%d", __LINE__, __func__,
             addr, cmd_ret_payload.retBuf[0]);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retBuf[1] == 0 || cmd_ret_payload.retBuf[1] == 2) {
    if (cmd_ret_payload.retBuf[1] == 0)
      *state = true;

    if (cmd_ret_payload.retBuf[1] == 2)
      *state = false;
  } else {
    ESP_LOGE(TAG, "%4d %s addr:%d error ret buf 1:%d", __LINE__, __func__, addr,
             cmd_ret_payload.retBuf[1]);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // ESP_LOGI(TAG, "%4d %s addr:%d success state:%d", __LINE__, __func__, addr,
  //*state);

  return ESP_OK;
}

esp_err_t modbus_ld100_get_relay(int addr, bool *state) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  err = modbus_sync_Cmd_04(addr, LD100_CMD_DEV_RELAY, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    return err;
  }

  if (cmd_ret_payload.recvCmd != LD100_READ) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
             addr, cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != 2) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
             addr, cmd_ret_payload.retLen);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retBuf[0] != 0) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret buf 0:%d", __LINE__, __func__,
             addr, cmd_ret_payload.retBuf[0]);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retBuf[1] == 0 || cmd_ret_payload.retBuf[1] == 1) {
    if (cmd_ret_payload.retBuf[1] == 0)
      *state = false;

    if (cmd_ret_payload.retBuf[1] == 1)
      *state = true;
  } else {
    ESP_LOGE(TAG, "%4d %s addr:%d error ret buf 1:%d", __LINE__, __func__, addr,
             cmd_ret_payload.retBuf[1]);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // ESP_LOGI(TAG, "%4d %s addr:%d success state:%d", __LINE__, __func__, addr,
  //        *state);

  return ESP_OK;
}

esp_err_t modbus_ld100_set_relay(int addr, bool state) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;
  USHORT target_state;

  if (state == true) {
    target_state = 1;
  } else {
    target_state = 0;
  }

  err = modbus_sync_Cmd_06(addr, LD100_CMD_DEV_RELAY, target_state,
                           &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    return err;
  }

  if (cmd_ret_payload.recvCmd != LD100_WRITE) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
             addr, cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != 2) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
             addr, cmd_ret_payload.retLen);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retBuf[0] != 0) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret buf 0:%d", __LINE__, __func__,
             addr, cmd_ret_payload.retBuf[0]);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retBuf[1] == 0 || cmd_ret_payload.retBuf[1] == 1) {
    if (cmd_ret_payload.retBuf[1] != state) {
      ESP_LOGE(TAG, "%4d %s addr:%d target:%d ret:%d", __LINE__, __func__, addr,
               state, cmd_ret_payload.retBuf[1]);
      return ESP_ERR_INVALID_RESPONSE;
    }
  } else {
    ESP_LOGE(TAG, "%4d %s addr:%d error ret buf 1:%d", __LINE__, __func__, addr,
             cmd_ret_payload.retBuf[1]);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // ESP_LOGI(TAG, "%4d %s addr:%d success state:%d", __LINE__, __func__, addr,
  //         state);

  return ESP_OK;
}

esp_err_t modbus_ld100_init(uint8_t port, int tx_pin, int rx_pin, int en_pin) {
  eMBErrorCode emb_ret = 0;
  ULONG RS485_BAUD = 9600;
  eMBParity RS485_PARITY = MB_PAR_NONE;

  emb_ret = modbus_init(port, RS485_BAUD, RS485_PARITY, tx_pin, rx_pin, en_pin);
  if (emb_ret != 0) {
    ESP_LOGE(TAG, "%4d %s modbus_init failed", __LINE__, __func__);
    return emb_ret;
  }

  mt_vMBMaster_set_T35_interval(125);
  mt_modbus_task();

  return ESP_OK;
}

mt_module_flow_struct_group_t *modbus_ld100_get_data(uint8_t port) {
  esp_err_t err = ESP_OK;
  bool state = false;
  int count = 0;

  mt_module_flow_struct_group_t *data_out = mt_module_flow_new_struct_group(1);

  // key
  data_out->value[count++]->key = mt_utils_string_copy("state1");
  count = 0;

  // get state
  err = modbus_ld100_get_state(port, &state);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d modbus_ld100_get_state failed", __LINE__,
             __func__, port);
    goto EXIT;
  } else {
    data_out->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE;
    data_out->value[count]->bool_value = state;
  }

EXIT:
  if (err != ESP_OK) {
    mt_module_flow_free_struct_group(data_out);
    data_out = NULL;
  }
  return data_out;
}
