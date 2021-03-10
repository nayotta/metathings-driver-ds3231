#include "modbus_aew100.h"

#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "mt_mbfunc.h"
#include "mt_mbtask.h"
#include "mt_port.h"

#include "mt_nvs_storage.h"
#include "mt_utils_byte.h"

// global define ==============================================================

static const char *TAG = "MT_MODBUS_AEW100";

static int Device_num = 1;

#define MODBUS_READ 03
#define MODBUS_WRITE 10
#define DATA_CURRENTA 0x1A
#define DATA_CURRENTB 0x1B
#define DATA_CURRENTC 0x1C
#define DATA_VOTAGEA 0x14
#define DATA_VOTAGEB 0x15
#define DATA_VOTAGEC 0x16
#define DATA_ACTIVEPOWERA 0x1E
#define DATA_ACTIVEPOWERB 0x20
#define DATA_ACTIVEPOWERC 0x22
#define DATA_REACTIVEPOWERA 0x26
#define DATA_REACTIVEPOWERB 0x28
#define DATA_REACTIVEPOWERC 0x2A
#define DATA_POWERFACTORA 0x36
#define DATA_POWERFACTORB 0x37
#define DATA_POWERFACTORC 0x38
#define DATA_QUALITYA 0x46
#define DATA_QUALITYB 0x50
#define DATA_QUALITYC 0x5A
#define DATA_TEMPA 0x0164
#define DATA_TEMPB 0x0165
#define DATA_TEMPC 0x0166

// global func ================================================================

esp_err_t modbus_aew100_get_current_ABC(uint8_t addr, double *a, double *b,
                                        double *c) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;
  int data_size = 3;

  // current A
  err = modbus_sync_Cmd_03(addr, DATA_CURRENTA, data_size, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2 * data_size) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *a = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 100.0;
    *b = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 2) / 100.0;
    *c = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 4) / 100.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_currentA(uint8_t addr, double *current) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // current A
  err = modbus_sync_Cmd_03(addr, DATA_CURRENTA, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *current = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 100.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_currentB(uint8_t addr, double *current) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // current B
  err = modbus_sync_Cmd_03(addr, DATA_CURRENTB, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *current = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 100.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_currentC(uint8_t addr, double *current) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // current C
  err = modbus_sync_Cmd_03(addr, DATA_CURRENTC, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *current = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 100.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_votage_ABC(uint8_t addr, double *a, double *b,
                                       double *c) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;
  int data_size = 3;

  // votage ABC
  err = modbus_sync_Cmd_03(addr, DATA_VOTAGEA, data_size, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2 * data_size) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *a = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 10.0;
    *b = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 2) / 10.0;
    *c = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 4) / 10.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_votageA(uint8_t addr, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // votage A
  err = modbus_sync_Cmd_03(addr, DATA_VOTAGEA, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 10.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_votageB(uint8_t addr, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // votage B
  err = modbus_sync_Cmd_03(addr, DATA_VOTAGEB, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 10.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_votageC(uint8_t addr, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // votage C
  err = modbus_sync_Cmd_03(addr, DATA_VOTAGEC, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 10.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_activePower_ABC(uint8_t addr, double *a, double *b,
                                            double *c) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;
  int data_size = 3;

  // activePowerA
  err = modbus_sync_Cmd_03(addr, DATA_ACTIVEPOWERA, 2 * data_size,
                           &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 4 * data_size) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *a = mt_utils_byte_4char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
    *b = mt_utils_byte_4char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
    *c = mt_utils_byte_4char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
    if (cmd_ret_payload.retBuf[0] == 0xff) {
      *a = 0;
    }
    if (cmd_ret_payload.retBuf[4] == 0xff) {
      *b = 0;
    }
    if (cmd_ret_payload.retBuf[8] == 0xff) {
      *c = 0;
    }
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_activePowerA(uint8_t addr, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // activePowerA
  err = modbus_sync_Cmd_03(addr, DATA_ACTIVEPOWERA, 2, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 4) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = mt_utils_byte_4char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
    if (cmd_ret_payload.retBuf[0] == 0xff) {
      *data = 0;
    }
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_activePowerB(uint8_t addr, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // activePowerB
  err = modbus_sync_Cmd_03(addr, DATA_ACTIVEPOWERB, 2, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 4) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = mt_utils_byte_4char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;

    if (cmd_ret_payload.retBuf[0] == 0xff) {
      *data = 0;
    }
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_activePowerC(uint8_t addr, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // activePowerC
  err = modbus_sync_Cmd_03(addr, DATA_ACTIVEPOWERC, 2, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 4) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = mt_utils_byte_4char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;

    if (cmd_ret_payload.retBuf[0] == 0xff) {
      *data = 0;
    }
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_reactivePower_ABC(uint8_t addr, double *a,
                                              double *b, double *c) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;
  int data_size = 3;

  // reactivePowerA
  err = modbus_sync_Cmd_03(addr, DATA_REACTIVEPOWERA, 2 * data_size,
                           &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 4 * data_size) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *a = mt_utils_byte_4char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
    *b = mt_utils_byte_4char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
    *c = mt_utils_byte_4char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_reactivePowerA(uint8_t addr, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // reactivePowerA
  err = modbus_sync_Cmd_03(addr, DATA_REACTIVEPOWERA, 2, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 4) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = mt_utils_byte_4char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_reactivePowerB(uint8_t addr, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // reactivePowerB
  err = modbus_sync_Cmd_03(addr, DATA_REACTIVEPOWERB, 2, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 4) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = mt_utils_byte_4char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_reactivePowerC(uint8_t addr, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // reactivePowerC
  err = modbus_sync_Cmd_03(addr, DATA_REACTIVEPOWERA, 2, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 4) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = mt_utils_byte_4char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_powerFactor_ABC(uint8_t addr, double *a, double *b,
                                            double *c) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;
  int data_size = 3;

  // powerFactorA
  err = modbus_sync_Cmd_03(addr, DATA_POWERFACTORA, 1 * data_size,
                           &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2 * data_size) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *a = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
    *b = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
    *c = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_powerFactorA(uint8_t addr, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // powerFactorA
  err = modbus_sync_Cmd_03(addr, DATA_POWERFACTORA, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_powerFactorB(uint8_t addr, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // powerFactorB
  err = modbus_sync_Cmd_03(addr, DATA_POWERFACTORB, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_powerFactorC(uint8_t addr, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // powerFactorC
  err = modbus_sync_Cmd_03(addr, DATA_POWERFACTORC, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_quality_ABC(uint8_t addr, double *a, double *b,
                                        double *c) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;
  int data_size = 3;

  // qualityA
  err =
      modbus_sync_Cmd_03(addr, DATA_QUALITYA, 1 * data_size, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2 * data_size) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *a = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
    *b = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
    *c = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_qualityA(uint8_t addr, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // qualityA
  err = modbus_sync_Cmd_03(addr, DATA_QUALITYA, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_qualityB(uint8_t addr, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // qualityB
  err = modbus_sync_Cmd_03(addr, DATA_QUALITYB, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_qualityC(uint8_t addr, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // qualityC
  err = modbus_sync_Cmd_03(addr, DATA_QUALITYC, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 1000.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_temp_ABC(uint8_t addr, double *a, double *b,
                                     double *c) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;
  int data_size = 3;

  // tempA
  err = modbus_sync_Cmd_03(addr, DATA_TEMPA, 1 * data_size, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2 * data_size) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *a = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 10.0;
    *b = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 10.0;
    *c = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 10.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_tempA(uint8_t addr, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // tempA
  err = modbus_sync_Cmd_03(addr, DATA_TEMPA, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 10.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_tempB(uint8_t addr, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // tempB
  err = modbus_sync_Cmd_03(addr, DATA_TEMPB, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 10.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_get_tempC(uint8_t addr, double *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // tempC
  err = modbus_sync_Cmd_03(addr, DATA_TEMPC, 1, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != MODBUS_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = mt_utils_byte_2char_to_int32(cmd_ret_payload.retBuf + 0) / 10.0;
  }

  return ESP_OK;
}

esp_err_t modbus_aew100_init(uint8_t port, int tx_pin, int rx_pin, int en_pin,
                             int device_num) {
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

  Device_num = device_num;

  return ESP_OK;
}

esp_err_t modbus_aew100_with_config_init(uint8_t port, int tx_pin, int rx_pin,
                                         int en_pin) {
  int32_t device_num = 0;
  esp_err_t err = ESP_OK;

  if (mt_nvs_read_int32_config("device_num", &device_num) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_read_int32_config device_num failed", __LINE__,
             __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  err = modbus_aew100_init(port, tx_pin, rx_pin, en_pin, device_num);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_aew100_init failed", __LINE__, __func__);
    return err;
  }

  return ESP_OK;
}

// json api func ==============================================================

cJSON *modbus_aew100_get_flow_data() {
  esp_err_t err = ESP_OK;
  cJSON *json_data = cJSON_CreateObject();
  char key[24] = "";
  bool exist = false;
  double currentA, currentB, currentC = 0;
  double voltageA, voltageB, voltageC = 0;
  double activePowerA, activePowerB, activePowerC = 0;
  double powerFactorA, powerFactorB, powerFactorC = 0;
  double tempA, tempB, tempC = 0;

  for (int i = 0; i < Device_num; i++) {
    // current ABC
    err = modbus_aew100_get_current_ABC(i + 1, &currentA, &currentB, &currentC);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s modbus_bits001_get_temp_hum failed", __LINE__,
               __func__);
      continue;
    } else {
      exist = true;
      // currentA
      sprintf(key, "IA%d", i + 1);
      cJSON_AddNumberToObject(json_data, key, currentA);

      // currentB
      sprintf(key, "IB%d", i + 1);
      cJSON_AddNumberToObject(json_data, key, currentB);

      // currentC
      sprintf(key, "IC%d", i + 1);
      cJSON_AddNumberToObject(json_data, key, currentC);
    }

    // voltage ABC
    err = modbus_aew100_get_votage_ABC(i + 1, &voltageA, &voltageB, &voltageC);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s modbus_aew100_get_votage_ABC failed", __LINE__,
               __func__);
      continue;
    } else {
      exist = true;
      // voltageA
      sprintf(key, "VA%d", i + 1);
      cJSON_AddNumberToObject(json_data, key, voltageA);

      // voltageB
      sprintf(key, "VB%d", i + 1);
      cJSON_AddNumberToObject(json_data, key, voltageB);

      // voltageC
      sprintf(key, "VC%d", i + 1);
      cJSON_AddNumberToObject(json_data, key, voltageC);
    }

    // activePower ABC
    err = modbus_aew100_get_activePower_ABC(i + 1, &activePowerA, &activePowerB,
                                            &activePowerC);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s modbus_aew100_get_activePower_ABC failed", __LINE__,
               __func__);
      continue;
    } else {
      exist = true;
      // activePowerA
      sprintf(key, "PA%d", i + 1);
      cJSON_AddNumberToObject(json_data, key, activePowerA);

      // activePowerB
      sprintf(key, "PB%d", i + 1);
      cJSON_AddNumberToObject(json_data, key, activePowerB);

      // activePowerC
      sprintf(key, "PC%d", i + 1);
      cJSON_AddNumberToObject(json_data, key, activePowerC);
    }

    // power fator ABC
    err = modbus_aew100_get_powerFactor_ABC(i + 1, &powerFactorA, &powerFactorB,
                                            &powerFactorC);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s modbus_aew100_get_powerFactor_ABC failed", __LINE__,
               __func__);
      continue;
    } else {
      exist = true;
      // powerFactorA
      sprintf(key, "FA%d", i + 1);
      cJSON_AddNumberToObject(json_data, key, powerFactorA);

      // powerFactorB
      sprintf(key, "FB%d", i + 1);
      cJSON_AddNumberToObject(json_data, key, powerFactorB);

      // powerFactorC
      sprintf(key, "FC%d", i + 1);
      cJSON_AddNumberToObject(json_data, key, powerFactorC);
    }

    // temp ABC
    err = modbus_aew100_get_temp_ABC(i + 1, &tempA, &tempB, &tempC);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s modbus_aew100_get_temp_ABC failed", __LINE__,
               __func__);
      continue;
    } else {
      exist = true;
      // tempA
      sprintf(key, "TA%d", i + 1);
      cJSON_AddNumberToObject(json_data, key, tempA);

      // tempB
      sprintf(key, "TB%d", i + 1);
      cJSON_AddNumberToObject(json_data, key, tempB);

      // tempC
      sprintf(key, "TC%d", i + 1);
      cJSON_AddNumberToObject(json_data, key, tempC);
    }
  }

  if (exist == false) {
    cJSON_Delete(json_data);
    json_data = NULL;
  }
  return json_data;
}
