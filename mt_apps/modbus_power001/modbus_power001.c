#include "modbus_power001.h"

#include "mt_mbfunc.h"
#include "mt_mbport.h"
#include "mt_mbtask.h"
#include "mt_port.h"

// static define ==============================================================

static const char *TAG = "MODBUS_POWER001";

#define POWER001_READ 03
#define POWER001_CMD_POWER 0x000A
#define POWER001_CMD_VOLTAGE 0x0300
#define POWER001_CMD_CURRENT 0x0303

static int PORT_NUM = 3;
static int PORT_ADDR[3] = {1, 2, 3};

// help func ==================================================================

modbus_power001_data_t *modbus_power001_new_data() {
  modbus_power001_data_t *data = malloc(sizeof(modbus_power001_data_t));
  data->quality = 0;
  data->voltage = 0;
  data->current = 0;

  return data;
}

void modbus_power001_free_data(modbus_power001_data_t *data) {
  if (data == NULL) {
    return;
  }

  free(data);
}

modbus_power001_datas_t *modbus_power001_new_datas(int size) {
  if (size <= 0) {
    ESP_LOGE(TAG, "%4d %s size error:%d", __LINE__, __func__, size);
    return NULL;
  }

  modbus_power001_datas_t *datas = malloc(sizeof(modbus_power001_datas_t));

  datas->port_num = size;
  datas->datas = malloc(size * sizeof(modbus_power001_data_t *));

  return datas;
}

void modbus_power001_free_datas(modbus_power001_datas_t *datas) {
  if (datas == NULL) {
    return;
  }

  if (datas->datas == NULL) {
    free(datas);
    return;
  }

  for (int i = 0; i < datas->port_num; i++) {
    if (datas->datas[i] != NULL) {
      modbus_power001_free_data(datas->datas[i]);
    }
  }

  free(datas->datas);

  free(datas);
}

// global func ================================================================

esp_err_t modbus_power001_get_quality(uint8_t port, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // power
  err = modbus_sync_Cmd_03(port, POWER001_CMD_POWER, 2, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, port);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != POWER001_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               port, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 4) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               port, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = (cmd_ret_payload.retBuf[0] * (1 << 24) +
             cmd_ret_payload.retBuf[1] * (1 << 16) +
             cmd_ret_payload.retBuf[2] * (1 << 8) + cmd_ret_payload.retBuf[3]) /
            100.0;
  }

  return ESP_OK;
}

esp_err_t modbus_power001_get_voltage(uint8_t port, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // voltage
  err = modbus_sync_Cmd_03(port, POWER001_CMD_VOLTAGE, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, port);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != POWER001_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               port, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               port, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            10.0;
  }

  return ESP_OK;
}

esp_err_t modbus_power001_get_current(uint8_t port, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // current
  err = modbus_sync_Cmd_03(port, POWER001_CMD_CURRENT, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, port);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != POWER001_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               port, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               port, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            100.0;
  }

  return ESP_OK;
}

modbus_power001_data_t *modbus_power001_get_data(int port) {
  esp_err_t err = ESP_OK;
  modbus_power001_data_t *data = malloc(sizeof(modbus_power001_data_t));

  // get quality
  err = modbus_power001_get_quality(port, &data->quality);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d modbus_power001_get_quality failed", __LINE__,
             __func__, port);
    goto EXIT;
  }

  // get voltage
  err = modbus_power001_get_voltage(port, &data->voltage);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d modbus_power001_get_voltage failed", __LINE__,
             __func__, port);
    goto EXIT;
  }

  // get current
  err = modbus_power001_get_current(port, &data->current);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d modbus_power001_get_current failed", __LINE__,
             __func__, port);
    goto EXIT;
  }

EXIT:
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    modbus_power001_free_data(data);
    data = NULL;
  }
  return data;
}

modbus_power001_datas_t *modbus_power001_get_datas() {
  modbus_power001_datas_t *datas = modbus_power001_new_datas(PORT_NUM);

  for (int i = 0; i < PORT_NUM; i++) {
    datas->datas[i] = modbus_power001_get_data(PORT_ADDR[i]);
    if (datas->datas[i] == NULL) {
      ESP_LOGW(TAG,
               "%4d %s modbus_power001_get_data addr:%d real_addr:%d failed",
               __LINE__, __func__, i, PORT_ADDR[i]);
    }
  }

  return datas;
}

esp_err_t modbus_power001_init(int tx_pin, int rx_pin, int en_pin) {
  eMBErrorCode emb_ret = 0;
  UCHAR RS485_PORT = 2;
  ULONG RS485_BAUD = 2400;
  eMBParity RS485_PARITY = MB_PAR_EVEN;

  emb_ret =
      modbus_init(RS485_PORT, RS485_BAUD, RS485_PARITY, tx_pin, rx_pin, en_pin);
  if (emb_ret != 0) {
    ESP_LOGE(TAG, "%4d %s modbus_init failed", __LINE__, __func__);
    return emb_ret;
  }
    mt_vMBMaster_set_T35_interval(250);
  mt_modbus_task();

  ESP_LOGI(TAG, "%4d %s init success", __LINE__, __func__);

  return ESP_OK;
}
