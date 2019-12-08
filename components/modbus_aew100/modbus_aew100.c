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

    *current =
        (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
        100.0;
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

    *current =
        (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
        100.0;
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

    *current =
        (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
        100.0;
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

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            10.0;
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

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            10.0;
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

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            10.0;
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

    *data = (cmd_ret_payload.retBuf[0] * (1 << 24) +
             cmd_ret_payload.retBuf[1] * (1 << 16) +
             cmd_ret_payload.retBuf[2] * (1 << 8) + cmd_ret_payload.retBuf[3]) /
            1000.0;
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

    *data = (cmd_ret_payload.retBuf[0] * (1 << 24) +
             cmd_ret_payload.retBuf[1] * (1 << 16) +
             cmd_ret_payload.retBuf[2] * (1 << 8) + cmd_ret_payload.retBuf[3]) /
            1000.0;

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

    *data = (cmd_ret_payload.retBuf[0] * (1 << 24) +
             cmd_ret_payload.retBuf[1] * (1 << 16) +
             cmd_ret_payload.retBuf[2] * (1 << 8) + cmd_ret_payload.retBuf[3]) /
            1000.0;

    if (cmd_ret_payload.retBuf[0] == 0xff) {
      *data = 0;
    }
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

    *data = (cmd_ret_payload.retBuf[0] * (1 << 24) +
             cmd_ret_payload.retBuf[1] * (1 << 16) +
             cmd_ret_payload.retBuf[2] * (1 << 8) + cmd_ret_payload.retBuf[3]) /
            1000.0;
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

    *data = (cmd_ret_payload.retBuf[0] * (1 << 24) +
             cmd_ret_payload.retBuf[1] * (1 << 16) +
             cmd_ret_payload.retBuf[2] * (1 << 8) + cmd_ret_payload.retBuf[3]) /
            1000.0;
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

    *data = (cmd_ret_payload.retBuf[0] * (1 << 24) +
             cmd_ret_payload.retBuf[1] * (1 << 16) +
             cmd_ret_payload.retBuf[2] * (1 << 8) + cmd_ret_payload.retBuf[3]) /
            1000.0;
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

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            1000.0;
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

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            1000.0;
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

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            1000.0;
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

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            1000.0;
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

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            1000.0;
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

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            1000.0;
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

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            10.0;
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

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            10.0;
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

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            10.0;
  }

  return ESP_OK;
}

esp_err_t mt_aew100_get_data(UCHAR addr, Aew100_data_t *data) {
  esp_err_t err = ESP_OK;

  // TODO(ZH) need lock
  err = mt_aew100_get_currentA(addr, &data->currentA);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_currentA failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_currentB(addr, &data->currentB);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_currentB failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_currentC(addr, &data->currentC);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_currentC failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_votageA(addr, &data->votageA);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_votageA failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_votageB(addr, &data->votageB);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_votageB failed", __LINE__,
             __func__, addr);
    return err;
  }
  err = mt_aew100_get_votageC(addr, &data->votageC);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_votageC failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_activePowerA(addr, &data->activePowerA);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_activePowerA failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_activePowerB(addr, &data->activePowerB);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_activePowerB failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_activePowerC(addr, &data->activePowerC);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_activePowerC failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_reactivePowerA(addr, &data->reactivePowerA);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_reactivePowerA failed",
             __LINE__, __func__, addr);
    return err;
  }

  err = mt_aew100_get_reactivePowerB(addr, &data->reactivePowerB);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_reactivePowerB failed",
             __LINE__, __func__, addr);
    return err;
  }

  err = mt_aew100_get_reactivePowerC(addr, &data->reactivePowerC);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_reactivePowerC failed",
             __LINE__, __func__, addr);
    return err;
  }

  err = mt_aew100_get_powerFactorA(addr, &data->powerFactorA);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_powerFactorA failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_powerFactorB(addr, &data->powerFactorB);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_powerFactorB failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_powerFactorC(addr, &data->powerFactorC);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_powerFactorC failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_qualityA(addr, &data->qualityA);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_qualityA failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_qualityB(addr, &data->qualityB);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_qualityB failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_qualityC(addr, &data->qualityC);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_qualityC failed", __LINE__,
             __func__, addr);
    return err;
  }

  return ESP_OK;
}

esp_err_t mt_aew100_get_data2(UCHAR addr, Aew100_data_t *data) {
  esp_err_t err = ESP_OK;

  err = mt_aew100_get_currentA(addr, &data->currentA);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_currentA failed", __LINE__,
             __func__, addr);
    goto EXIT;
  } else {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_currentA success", __LINE__,
             __func__, addr);
  }

  err = mt_aew100_get_currentB(addr, &data->currentB);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_currentB failed", __LINE__,
             __func__, addr);
    goto EXIT;
  }

  err = mt_aew100_get_currentC(addr, &data->currentC);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_currentC failed", __LINE__,
             __func__, addr);
    goto EXIT;
  }

  err = mt_aew100_get_votageA(addr, &data->votageA);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_votageA failed", __LINE__,
             __func__, addr);
    goto EXIT;
  }

  err = mt_aew100_get_votageB(addr, &data->votageB);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_votageB failed", __LINE__,
             __func__, addr);
    goto EXIT;
  }

  err = mt_aew100_get_votageC(addr, &data->votageC);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_votageC failed", __LINE__,
             __func__, addr);
    goto EXIT;
  }

  err = mt_aew100_get_tempA(addr, &data->tempA);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_tempA failed", __LINE__,
             __func__, addr);
    goto EXIT;
  }

  err = mt_aew100_get_tempB(addr, &data->tempB);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_tempB failed", __LINE__,
             __func__, addr);
    goto EXIT;
  }

  err = mt_aew100_get_tempC(addr, &data->tempC);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_tempC failed", __LINE__,
             __func__, addr);
    goto EXIT;
  }

  data->powerFactorA = 100.0;
  data->powerFactorB = 100.0;
  data->powerFactorC = 100.0;

  data->activePowerA = data->votageA * data->currentA;
  data->activePowerB = data->votageB * data->currentB;
  data->activePowerC = data->votageC * data->currentC;

EXIT:
  return err;
}

esp_err_t mt_aew100_init(int tx_pin, int rx_pin, int en_pin) {
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

  mt_vMBMaster_set_T35_interval(250);
  mt_modbus_task();

  return ESP_OK;
}
