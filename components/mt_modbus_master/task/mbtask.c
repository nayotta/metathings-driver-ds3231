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
static const char *TAG = "MT_MODBUS_MASTER_TASK";

static SemaphoreHandle_t SemaphorMasterHdl = NULL;
static int Lock_Timeout = 50;

// static func ================================================================
static void modbus_lock_init() {
  SemaphorMasterHdl = xSemaphoreCreateMutex();
  return;
}

static bool modbus_lock_take(LONG timeout) {
  if (xSemaphoreTake(SemaphorMasterHdl, (portTickType)timeout) == pdTRUE) {
    return true;
  }
  return false;
}

static void modbus_lock_release() {
  xSemaphoreGive(SemaphorMasterHdl);
  return;
}

// global func ================================================================
// cmd 01 callback
eMBErrorCode eMBMasterCB01(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen) {
  eMBErrorCode eStatus = MB_ENOERR;

  if (recvLen > MT_MB_TASK_BUF_MAXLEN) {
    return MB_EILLSTATE;
  }

  memcpy(RetMsg->retBuf, recvBuf, recvLen);
  RetMsg->recvCmd = recvCmd;
  RetMsg->retLen = recvLen;

  return eStatus;
}

// cmd 02 callback
eMBErrorCode eMBMasterCB02(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen) {
  eMBErrorCode eStatus = MB_ENOERR;

  if (recvLen > MT_MB_TASK_BUF_MAXLEN) {
    return MB_EILLSTATE;
  }

  memcpy(RetMsg->retBuf, recvBuf, recvLen);
  RetMsg->recvCmd = recvCmd;
  RetMsg->retLen = recvLen;

  return eStatus;
}

// cmd 03 callback
eMBErrorCode eMBMasterCB03(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen) {
  eMBErrorCode eStatus = MB_ENOERR;

  if (recvLen > MT_MB_TASK_BUF_MAXLEN) {
    return MB_EILLSTATE;
  }

  memcpy(RetMsg->retBuf, recvBuf, recvLen);
  RetMsg->recvCmd = recvCmd;
  RetMsg->retLen = recvLen;

  return eStatus;
}

// cmd 04 callback
eMBErrorCode eMBMasterCB04(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen) {
  eMBErrorCode eStatus = MB_ENOERR;

  if (recvLen > MT_MB_TASK_BUF_MAXLEN) {
    return MB_EILLSTATE;
  }

  memcpy(RetMsg->retBuf, recvBuf, recvLen);
  RetMsg->recvCmd = recvCmd;
  RetMsg->retLen = recvLen;

  return eStatus;
}

// cmd 05 callback
eMBErrorCode eMBMasterCB05(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen) {
  eMBErrorCode eStatus = MB_ENOERR;

  if (recvLen > MT_MB_TASK_BUF_MAXLEN) {
    return MB_EILLSTATE;
  }

  memcpy(RetMsg->retBuf, recvBuf, recvLen);
  RetMsg->recvCmd = recvCmd;
  RetMsg->retLen = recvLen;

  return eStatus;
}

// cmd 06 callback
eMBErrorCode eMBMasterCB06(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen) {
  eMBErrorCode eStatus = MB_ENOERR;

  if (recvLen > MT_MB_TASK_BUF_MAXLEN) {
    return MB_EILLSTATE;
  }

  memcpy(RetMsg->retBuf, recvBuf, recvLen);
  RetMsg->recvCmd = recvCmd;
  RetMsg->retLen = recvLen;

  return eStatus;
}

eMBErrorCode modbus_init(UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity,
                         int tx_pin, int rx_pin, int en_pin) {
  eMBErrorCode ret = 0;

  // default 9600 8in1  config
  ret = eMBMasterInit(MB_RTU, ucPort, ulBaudRate, eParity, tx_pin, rx_pin,
                      en_pin);
  if (ret != 0) {
    ESP_LOGE(TAG, "%4d eMBInit failed!!! eStatus: %d", __LINE__, ret);
  }

  return ret;
}

// modbus main loop
static void modbus_loop(void *parameter) {
  eMBErrorCode eStatus;

  // theard lock
  modbus_lock_init();

  RetMsg = malloc(sizeof(struct RetMsg_t)); // global no need free
  memset(RetMsg->retBuf, 0, MT_MB_TASK_BUF_MAXLEN);
  RetMsg->recvCmd = 0;
  RetMsg->retLen = 0;

  // master enable
  ESP_LOGI(TAG, "%4d %s eMBInit OK.", __LINE__, __func__);
  eStatus = eMBMasterEnable();
  if (eStatus != 0) {
    ESP_LOGE(TAG, "%4d %s eMBEnable failed!!! eStatus: %d", __LINE__, __func__,
             eStatus);
  }

  // master loop
  ESP_LOGI(TAG, "%4d %s starting eMBMasterPoll...", __LINE__, __func__);
  while (1) {
    // (TODO zh) if need delay?
    eMBMasterPoll();
    vTaskDelay(1);
  }

  vTaskDelete(NULL);
}

// sync cmd 01
eMBMasterReqErrCode modbus_sync_Cmd_01(UCHAR slaveAddr, USHORT target,
                                       USHORT num, struct RetMsg_t *ret) {
  eMBMasterReqErrCode errorCode = MB_MRE_NO_ERR;

  if (modbus_lock_take(Lock_Timeout) == false) {
    errorCode = MB_MRE_MASTER_BUSY;
    ESP_LOGE(TAG, "%4d eMBsend get lock timeout", __LINE__);
    return errorCode;
  }

  errorCode = eMBMasterReq01(slaveAddr, target, num, 1);
  if (errorCode != MB_MRE_NO_ERR) {
    ESP_LOGE(TAG, "%4d eMBsend error", __LINE__);
    goto EXIT;
  }

  ret->recvCmd = RetMsg->recvCmd;
  ret->retLen = RetMsg->retLen;
  memcpy(ret->retBuf, RetMsg->retBuf, RetMsg->retLen);

EXIT:
  modbus_lock_release();
  return errorCode;
}

// sync cmd 02
eMBMasterReqErrCode modbus_sync_Cmd_02(UCHAR slaveAddr, USHORT target,
                                       USHORT num, struct RetMsg_t *ret) {
  eMBMasterReqErrCode errorCode = MB_MRE_NO_ERR;

  if (modbus_lock_take(Lock_Timeout) == false) {
    errorCode = MB_MRE_MASTER_BUSY;
    ESP_LOGE(TAG, "%4d eMBsend get lock timeout", __LINE__);
    return errorCode;
  }

  errorCode = eMBMasterReq02(slaveAddr, target, num, 1);
  if (errorCode != MB_MRE_NO_ERR) {
    ESP_LOGE(TAG, "%4d eMBsend error", __LINE__);
    goto EXIT;
  }

  ret->recvCmd = RetMsg->recvCmd;
  ret->retLen = RetMsg->retLen;
  memcpy(ret->retBuf, RetMsg->retBuf, RetMsg->retLen);

EXIT:
  modbus_lock_release();
  return errorCode;
}

// sync cmd 03
eMBMasterReqErrCode modbus_sync_Cmd_03(UCHAR slaveAddr, USHORT target,
                                       USHORT num, struct RetMsg_t *ret) {
  eMBMasterReqErrCode errorCode = MB_MRE_NO_ERR;

  if (modbus_lock_take(Lock_Timeout) == false) {
    errorCode = MB_MRE_MASTER_BUSY;
    ESP_LOGE(TAG, "%4d eMBsend get lock timeout", __LINE__);
    return errorCode;
  }

  errorCode = eMBMasterReq03(slaveAddr, target, num, 1);
  if (errorCode != MB_MRE_NO_ERR) {
    ESP_LOGE(TAG, "%4d eMBsend error", __LINE__);
    goto EXIT;
  }

  ret->recvCmd = RetMsg->recvCmd;
  ret->retLen = RetMsg->retLen;
  memcpy(ret->retBuf, RetMsg->retBuf, RetMsg->retLen);

EXIT:
  modbus_lock_release();
  return errorCode;
}

// sync cmd 04
eMBMasterReqErrCode modbus_sync_Cmd_04(UCHAR slaveAddr, USHORT target,
                                       USHORT num, struct RetMsg_t *ret) {
  eMBMasterReqErrCode errorCode = MB_MRE_NO_ERR;

  if (modbus_lock_take(Lock_Timeout) == false) {
    errorCode = MB_MRE_MASTER_BUSY;
    ESP_LOGE(TAG, "%4d eMBsend get lock timeout", __LINE__);
    return errorCode;
  }

  errorCode = eMBMasterReq04(slaveAddr, target, num, 1);
  if (errorCode != MB_MRE_NO_ERR) {
    ESP_LOGE(TAG, "%4d eMBsend error", __LINE__);
    goto EXIT;
  }

  ret->recvCmd = RetMsg->recvCmd;
  ret->retLen = RetMsg->retLen;
  memcpy(ret->retBuf, RetMsg->retBuf, RetMsg->retLen);

EXIT:
  modbus_lock_release();
  return errorCode;
}

// sync cmd 05
eMBMasterReqErrCode modbus_sync_Cmd_05(UCHAR slaveAddr, USHORT target,
                                       USHORT num, struct RetMsg_t *ret) {
  eMBMasterReqErrCode errorCode = MB_MRE_NO_ERR;

  if (modbus_lock_take(Lock_Timeout) == false) {
    errorCode = MB_MRE_MASTER_BUSY;
    ESP_LOGE(TAG, "%4d eMBsend get lock timeout", __LINE__);
    return errorCode;
  }

  errorCode = eMBMasterReq05(slaveAddr, target, num, 1);
  if (errorCode != MB_MRE_NO_ERR) {
    ESP_LOGE(TAG, "%4d eMBsend error", __LINE__);
    goto EXIT;
  }

  ret->recvCmd = RetMsg->recvCmd;
  ret->retLen = RetMsg->retLen;
  memcpy(ret->retBuf, RetMsg->retBuf, RetMsg->retLen);

EXIT:
  modbus_lock_release();
  return errorCode;
}

// sync cmd 06
eMBMasterReqErrCode modbus_sync_Cmd_06(UCHAR slaveAddr, USHORT target,
                                       USHORT num, struct RetMsg_t *ret) {
  eMBMasterReqErrCode errorCode = MB_MRE_NO_ERR;

  if (modbus_lock_take(Lock_Timeout) == false) {
    errorCode = MB_MRE_MASTER_BUSY;
    ESP_LOGE(TAG, "%4d eMBsend get lock timeout", __LINE__);
    return errorCode;
  }

  errorCode = eMBMasterReq06(slaveAddr, target, num, 1);
  if (errorCode != MB_MRE_NO_ERR) {
    ESP_LOGE(TAG, "%4d eMBsend error", __LINE__);
    goto EXIT;
  }

  ret->recvCmd = RetMsg->recvCmd;
  ret->retLen = RetMsg->retLen;
  memcpy(ret->retBuf, RetMsg->retBuf, RetMsg->retLen);

EXIT:
  modbus_lock_release();
  return errorCode;
}

void mt_modbus_task() {
  // task level must be highest 12!!!!!!!!!
  xTaskCreatePinnedToCore(modbus_loop, "mt_modbus_task", 1024 * 8, NULL, 10,
                          NULL, 1);
}
