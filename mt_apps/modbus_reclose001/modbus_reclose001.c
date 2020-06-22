#include "modbus_reclose001.h"

#include "mt_mbfunc.h"
#include "mt_mbport.h"
#include "mt_mbtask.h"
#include "mt_port.h"

#include "mt_module_flow.h"
#include "mt_utils_string.h"

// static define ==============================================================

static const char *TAG = "MODBUS_RECLOSE001";

#define RECLOSE001_READ 03
#define RECLOSE001_CMD_STATE 0x0001
#define RECLOSE001_CMD_WARN 0x0051

// global func ================================================================

mt_module_flow_struct_group_t *modbus_reclose001_get_data(uint8_t port) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;
  mt_module_flow_struct_group_t *data_out = mt_module_flow_new_struct_group(7);
  int count = 0;

  // key
  data_out->value[count++]->key = mt_utils_string_copy("addr");
  data_out->value[count++]->key = mt_utils_string_copy("state");
  data_out->value[count++]->key = mt_utils_string_copy("warnHighVoltage");
  data_out->value[count++]->key = mt_utils_string_copy("warnLowVoltage");
  data_out->value[count++]->key = mt_utils_string_copy("warnOverload");
  data_out->value[count++]->key = mt_utils_string_copy("warnShort");
  data_out->value[count++]->key = mt_utils_string_copy("warnLeak");
  count = 0;

  // addr
  data_out->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  data_out->value[count]->number_value = port;
  data_out->value[count]->use_default = false;
  data_out->value[count++]->default_double_value = 0;

  // state
  err = modbus_sync_Cmd_03(port, RECLOSE001_CMD_STATE, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, port);
    goto EXIT;
  } else {
    if (cmd_ret_payload.recvCmd != RECLOSE001_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               port, cmd_ret_payload.recvCmd);
      err = ESP_ERR_INVALID_RESPONSE;
      goto EXIT;
    }

    if (cmd_ret_payload.retLen != 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               port, cmd_ret_payload.retLen);
      err = ESP_ERR_INVALID_RESPONSE;
      goto EXIT;
    }

    data_out->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE;
    data_out->value[count]->default_bool_value = true;
    if (cmd_ret_payload.retBuf[1] == 0) {
      data_out->value[count]->bool_value = false;
    } else {
      data_out->value[count]->bool_value = true;
    }
    count++;
  }

  // warn
  err = modbus_sync_Cmd_03(port, RECLOSE001_CMD_WARN, 5, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, port);
    goto EXIT;
  } else {
    if (cmd_ret_payload.recvCmd != RECLOSE001_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               port, cmd_ret_payload.recvCmd);
      err = ESP_ERR_INVALID_RESPONSE;
      goto EXIT;
    }

    if (cmd_ret_payload.retLen != 5 * 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               port, cmd_ret_payload.retLen);
      err = ESP_ERR_INVALID_RESPONSE;
      goto EXIT;
    }

    for (int i = 0; i < 5; i++) {
      data_out->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE;
      data_out->value[count]->default_bool_value = false;
      if (cmd_ret_payload.retBuf[1 + i * 2] == 0) {
        data_out->value[count]->bool_value = false;
      } else {
        data_out->value[count]->bool_value = true;
      }
      count++;
    }

  EXIT:
    if (err != ESP_OK) {
      mt_module_flow_free_struct_group(data_out);
      data_out = NULL;
    }
    return data_out;
  }
}

esp_err_t modbus_reclose001_task(uint8_t port, int tx_pin, int rx_pin,
                                 int en_pin) {
  eMBErrorCode emb_ret = 0;
  ULONG RS485_BAUD = 9600;
  eMBParity RS485_PARITY = MB_PAR_NONE;

  emb_ret = modbus_init(port, RS485_BAUD, RS485_PARITY, tx_pin, rx_pin, en_pin);
  if (emb_ret != 0) {
    ESP_LOGE(TAG, "%4d %s modbus_init failed", __LINE__, __func__);
    return emb_ret;
  }
  mt_vMBMaster_set_T35_interval(250);
  mt_modbus_task();

  ESP_LOGI(TAG, "%4d %s init success", __LINE__, __func__);

  return ESP_OK;
}
