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
#include "mt_utils_byte.h"

#include "modbus_aew100.h"
#include "mt_mbtask.h"

// global define ==============================================================

static const char *TAG = "MT_MODBUS_AEW100";

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

esp_err_t mt_aew100_get_current_ABC(UCHAR addr, double *a, double *b,
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

esp_err_t mt_aew100_get_currentA(UCHAR addr, double *current) {
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

esp_err_t mt_aew100_get_currentB(UCHAR addr, double *current) {
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

esp_err_t mt_aew100_get_currentC(UCHAR addr, double *current) {
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

esp_err_t mt_aew100_get_votage_ABC(UCHAR addr, double *a, double *b,
                                   double *c) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;
  int data_size = 3;

  // votage A
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

esp_err_t mt_aew100_get_votageA(UCHAR addr, double *data) {
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

esp_err_t mt_aew100_get_votageB(UCHAR addr, double *data) {
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

esp_err_t mt_aew100_get_votageC(UCHAR addr, double *data) {
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

esp_err_t mt_aew100_get_activePower_ABC(UCHAR addr, double *a, double *b,
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

esp_err_t mt_aew100_get_activePowerA(UCHAR addr, double *data) {
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

esp_err_t mt_aew100_get_activePowerB(UCHAR addr, double *data) {
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

esp_err_t mt_aew100_get_activePowerC(UCHAR addr, double *data) {
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

esp_err_t mt_aew100_get_reactivePower_ABC(UCHAR addr, double *a, double *b,
                                          double *c) {
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

esp_err_t mt_aew100_get_reactivePowerA(UCHAR addr, double *data) {
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

esp_err_t mt_aew100_get_reactivePowerB(UCHAR addr, double *data) {
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

esp_err_t mt_aew100_get_reactivePowerC(UCHAR addr, double *data) {
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

esp_err_t mt_aew100_get_powerFactor_ABC(UCHAR addr, double *a, double *b,
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

esp_err_t mt_aew100_get_powerFactorA(UCHAR addr, double *data) {
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

esp_err_t mt_aew100_get_powerFactorB(UCHAR addr, double *data) {
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

esp_err_t mt_aew100_get_powerFactorC(UCHAR addr, double *data) {
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

esp_err_t mt_aew100_get_quality_ABC(UCHAR addr, double *a, double *b,
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

esp_err_t mt_aew100_get_qualityA(UCHAR addr, double *data) {
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

esp_err_t mt_aew100_get_qualityB(UCHAR addr, double *data) {
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

esp_err_t mt_aew100_get_qualityC(UCHAR addr, double *data) {
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

esp_err_t mt_aew100_get_temp_ABC(UCHAR addr, double *a, double *b, double *c) {
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

esp_err_t mt_aew100_get_tempA(UCHAR addr, double *data) {
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

esp_err_t mt_aew100_get_tempB(UCHAR addr, double *data) {
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

esp_err_t mt_aew100_get_tempC(UCHAR addr, double *data) {
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

esp_err_t mt_aew100_get_data(UCHAR addr, MtAew100__Data *data) {
  esp_err_t err = ESP_OK;

  // TODO(ZH) need lock
  err = mt_aew100_get_current_ABC(addr, &data->currenta, &data->currentb,
                                  &data->currentc);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_current_ABC failed", __LINE__,
             __func__, addr);
  } else {
    data->has_currenta = true;
    data->has_currentb = true;
    data->has_currentc = true;
  }

  err = mt_aew100_get_votage_ABC(addr, &data->votagea, &data->votageb,
                                 &data->votagec);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_votage_ABC failed", __LINE__,
             __func__, addr);
  } else {
    data->has_votagea = true;
    data->has_votageb = true;
    data->has_votagec = true;
  }

  err = mt_aew100_get_activePower_ABC(addr, &data->activepowera,
                                      &data->activepowerb, &data->activepowerc);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_activePower_ABC failed",
             __LINE__, __func__, addr);
  } else {
    data->has_activepowera = true;
    data->has_activepowerb = true;
    data->has_activepowerc = true;
  }

  err = mt_aew100_get_reactivePower_ABC(addr, &data->reactivepowera,
                                        &data->reactivepowerb,
                                        &data->reactivepowerc);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_reactivePower_ABC failed",
             __LINE__, __func__, addr);
  } else {
    data->has_reactivepowera = true;
    data->has_reactivepowerb = true;
    data->has_reactivepowerc = true;
  }

  err = mt_aew100_get_powerFactor_ABC(addr, &data->powerfactora,
                                      &data->powerfactorb, &data->powerfactorc);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_powerFactor_ABC failed",
             __LINE__, __func__, addr);
  } else {
    data->has_powerfactora = true;
    data->has_powerfactorb = true;
    data->has_powerfactorc = true;
  }

  err = mt_aew100_get_quality_ABC(addr, &data->qualitya, &data->qualityb,
                                  &data->qualityc);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_quality_ABC failed", __LINE__,
             __func__, addr);
  } else {
    data->has_qualitya = true;
    data->has_qualityb = true;
    data->has_qualityc = true;
  }

  err = mt_aew100_get_temp_ABC(addr, &data->tempa, &data->tempb, &data->tempc);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_temp_ABC failed", __LINE__,
             __func__, addr);
  } else {
    data->has_tempa = true;
    data->has_tempb = true;
    data->has_tempc = true;
  }

  err = mt_aew100_get_temp_ABC(addr, &data->maxpowera, &data->maxpowerb,
                               &data->maxpowerc);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_temp_ABC failed", __LINE__,
             __func__, addr);
  } else {
    data->has_maxpowera = true;
    data->has_maxpowerb = true;
    data->has_maxpowerc = true;
  }

  return ESP_OK;
}

esp_err_t mt_aew100_get_data2(UCHAR addr, MtAew100__Data *data) {
  esp_err_t err = ESP_OK;

  err = mt_aew100_get_current_ABC(addr, &data->currenta, &data->currenta,
                                  &data->currentc);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_current_ABC failed", __LINE__,
             __func__, addr);
  } else {
    data->has_currenta = true;
    data->has_currentb = true;
    data->has_currentc = true;
  }

  err = mt_aew100_get_votage_ABC(addr, &data->votagea, &data->votageb,
                                 &data->votagec);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_votage_ABC failed", __LINE__,
             __func__, addr);
  } else {
    data->has_votagea = true;
    data->has_votageb = true;
    data->has_votagec = true;

    data->has_powerfactora = true;
    data->has_powerfactorb = true;
    data->has_powerfactorc = true;
    data->powerfactora = 100.0;
    data->powerfactorb = 100.0;
    data->powerfactorc = 100.0;

    data->has_activepowera = true;
    data->has_activepowerb = true;
    data->has_activepowerc = true;
    data->activepowera = data->votagea * data->currenta;
    data->activepowerb = data->votageb * data->currentb;
    data->activepowerc = data->votagec * data->currentc;
  }

  err = mt_aew100_get_temp_ABC(addr, &data->tempa, &data->tempa, &data->tempc);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_temp_ABC failed", __LINE__,
             __func__, addr);
  } else {
    data->has_tempa = true;
    data->has_tempb = true;
    data->has_tempc = true;
  }

  return err;
}

esp_err_t mt_aew100_init(uint8_t port, int tx_pin, int rx_pin, int en_pin) {
  eMBErrorCode emb_ret = 0;
  ULONG RS485_BAUD = 9600;
  eMBParity RS485_PARITY = MB_PAR_NONE;

  emb_ret = modbus_init(port, RS485_BAUD, RS485_PARITY, tx_pin, rx_pin, en_pin);
  if (emb_ret != 0) {
    ESP_LOGE(TAG, "%4d %s modbus_init failed", __LINE__, __func__);
    return emb_ret;
  }

  mt_vMBMaster_set_T35_interval(250);
  // mt_modbus_task();

  return ESP_OK;
}
