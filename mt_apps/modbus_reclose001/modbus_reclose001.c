#include "modbus_reclose001.h"

#include "mt_mbfunc.h"
#include "mt_mbport.h"
#include "mt_mbtask.h"
#include "mt_port.h"

// static define ==============================================================

static const char *TAG = "MODBUS_RECLOSE001";

#define RECLOSE001_READ 03
#define RECLOSE001_CMD_STATE 0x0001
#define RECLOSE001_CMD_WARN 0x0051

// help func ==================================================================

modbus_reclose001_data_t *modbus_reclose001_new_data() {
  modbus_reclose001_data_t *data = malloc(sizeof(modbus_reclose001_data_t));
  data->state = false;
  data->if_change_state = false;
  data->warn_high_voltage = false;
  data->if_change_warn_high_voltage = false;
  data->warn_low_voltage = false;
  data->if_change_warn_low_voltage = false;
  data->warn_overload = false;
  data->if_change_warn_overload = false;
  data->warn_short = false;
  data->if_change_warn_short = false;
  data->warn_leak = false;
  data->if_change_warn_leak = false;

  return data;
}

void modbus_reclose001_free_data(modbus_reclose001_data_t *data) {
  if (data == NULL) {
    return;
  }

  free(data);
}

// global func ================================================================

esp_err_t modbus_reclose001_get_data(uint8_t port,
                                     modbus_reclose001_data_t *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // state
  err = modbus_sync_Cmd_03(port, RECLOSE001_CMD_STATE, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, port);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != RECLOSE001_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               port, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               port, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retBuf[1] == 0) {
      data->state = false;
    } else {
      data->state = true;
    }
  }

  // warn
  err = modbus_sync_Cmd_03(port, RECLOSE001_CMD_WARN, 5, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, port);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != RECLOSE001_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               port, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 5 * 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               port, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retBuf[1] == 0) {
      data->warn_high_voltage = false;
    } else {
      data->warn_high_voltage = true;
    }

    if (cmd_ret_payload.retBuf[3] == 0) {
      data->warn_low_voltage = false;
    } else {
      data->warn_low_voltage = true;
    }

    if (cmd_ret_payload.retBuf[5] == 0) {
      data->warn_overload = false;
    } else {
      data->warn_overload = true;
    }

    if (cmd_ret_payload.retBuf[7] == 0) {
      data->warn_short = false;
    } else {
      data->warn_short = true;
    }

    if (cmd_ret_payload.retBuf[9] == 0) {
      data->warn_leak = false;
    } else {
      data->warn_leak = true;
    }
  }

  return ESP_OK;
}

esp_err_t modbus_reclose001_get_data_and_check_if_change(
    uint8_t port, modbus_reclose001_data_t *data,
    modbus_reclose001_data_t *data_save, bool *change) {
  esp_err_t err = ESP_OK;
  *change = false;

  err = modbus_reclose001_get_data(port, data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d modbus_reclose001_get_data failed", __LINE__,
             __func__, port);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (data->state != data_save->state) {
    data_save->state = data->state;
    data_save->if_change_state = true;
    *change = true;
    ESP_LOGW(TAG, "%4d %s state change to %d", __LINE__, __func__,
             data_save->state);
  } else {
    data_save->if_change_state = false;
  }

  if (data->warn_high_voltage != data_save->warn_high_voltage) {
    data_save->warn_high_voltage = data->warn_high_voltage;
    data_save->if_change_warn_high_voltage = true;
    *change = true;
    ESP_LOGW(TAG, "%4d %s warn_high_voltage change to %d", __LINE__, __func__,
             data_save->warn_high_voltage);
  } else {
    data_save->if_change_warn_high_voltage = false;
  }

  if (data->warn_low_voltage != data_save->warn_low_voltage) {
    data_save->warn_low_voltage = data->warn_low_voltage;
    data_save->if_change_warn_low_voltage = true;
    *change = true;
    ESP_LOGW(TAG, "%4d %s warn_low_voltage change to %d", __LINE__, __func__,
             data_save->warn_low_voltage);
  } else {
    data_save->if_change_warn_low_voltage = false;
  }

  if (data->warn_overload != data_save->warn_overload) {
    data_save->warn_overload = data->warn_overload;
    data_save->if_change_warn_overload = true;
    *change = true;
    ESP_LOGW(TAG, "%4d %s warn_overload change to %d", __LINE__, __func__,
             data_save->warn_overload);
  } else {
    data_save->if_change_warn_overload = false;
  }

  if (data->warn_short != data_save->warn_short) {
    data_save->warn_short = data->warn_short;
    data_save->if_change_warn_short = true;
    *change = true;
    ESP_LOGW(TAG, "%4d %s warn_short change to %d", __LINE__, __func__,
             data_save->warn_short);
  } else {
    data_save->if_change_warn_short = false;
  }

  if (data->warn_leak != data_save->warn_leak) {
    data_save->warn_leak = data->warn_leak;
    data_save->if_change_warn_leak = true;
    *change = true;
    ESP_LOGW(TAG, "%4d %s warn_leak change to %d", __LINE__, __func__,
             data_save->warn_leak);
  } else {
    data_save->if_change_warn_leak = false;
  }

  return ESP_OK;
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
  mt_modbus_task();

  ESP_LOGI(TAG, "%4d %s init success", __LINE__, __func__);

  return ESP_OK;
}
