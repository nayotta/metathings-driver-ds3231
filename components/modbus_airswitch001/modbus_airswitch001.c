#include "esp_timer.h"
#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "cache_airswitch001.h"
#include "modbus_airswitch001.h"

// global define ==============================================================
static const char *TAG = "MODBUS_airswitch001";
static SemaphoreHandle_t SemaphorMasterHdl = NULL;
static int Lock_Timeout = 50;

extern int NUM_MASTER;
extern int NUM_SLAVER;

#define CMD_01_STAT_READ 0x01
#define CMD_02_CRTL_READ 0x02
#define CMD_03_DATA_READ 0x03
#define CMD_04_PARM_READ 0x04
#define CMD_05_STAT_WRITE 0x05
#define CMD_06_PARM_WRITE 0x06

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

  if (recvLen > BUF_MAXLEN) {
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

  if (recvLen > BUF_MAXLEN) {
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

  if (recvLen > BUF_MAXLEN) {
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

  if (recvLen > BUF_MAXLEN) {
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

  if (recvLen > BUF_MAXLEN) {
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

  if (recvLen > BUF_MAXLEN) {
    return MB_EILLSTATE;
  }

  memcpy(RetMsg->retBuf, recvBuf, recvLen);
  RetMsg->recvCmd = recvCmd;
  RetMsg->retLen = recvLen;

  return eStatus;
}

eMBErrorCode modbus_airswitch001_init(UCHAR ucPort, ULONG ulBaudRate,
                                      eMBParity eParity, int tx_pin, int rx_pin,
                                      int en_pin) {
  eMBErrorCode ret = 0;

  // default 9600 8in1 config
  ret = eMBMasterAirswitchInit(MB_RTU, ucPort, ulBaudRate, eParity, tx_pin,
                               rx_pin, en_pin);
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
  memset(RetMsg->retBuf, 0, BUF_MAXLEN);
  RetMsg->recvCmd = 0;
  RetMsg->retLen = 0;

  // master enable
  ESP_LOGI(TAG, "%4d eMBInit OK.", __LINE__);
  eStatus = eMBMasterAirswitchEnable();
  if (eStatus != 0) {
    ESP_LOGE(TAG, "%4d eMBEnable failed!!! eStatus: %d", __LINE__, eStatus);
  }

  // master loop
  ESP_LOGI(TAG, "%4d starting eMBMasterPoll...", __LINE__);
  while (1) {
    // (TODO zh) if need delay?
    eMBMasterAirswitchPoll();
    vTaskDelay(0 / portTICK_RATE_MS);
  }

  ESP_LOGE(TAG, "%4d eMBmasterPoll unexcept exit!!!", __LINE__);
  vTaskDelete(NULL);
}

// sync cmd 01
static eMBMasterReqErrCode
modbus_airswitch001_sync_cmd_01(UCHAR slaveAddr, USHORT target, USHORT num,
                                struct RetMsg_t *ret) {
  eMBMasterReqErrCode errorCode = MB_MRE_NO_ERR;

  if (modbus_lock_take(Lock_Timeout) == false) {
    errorCode = MB_MRE_MASTER_BUSY;
    ESP_LOGE(TAG, "%4d eMBsend get lock timeout", __LINE__);
    return errorCode;
  }

  errorCode = eMBMasterAirswitchReq01(slaveAddr, target, num, 1);
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
static eMBMasterReqErrCode
modbus_airswitch001_sync_cmd_02(UCHAR slaveAddr, USHORT target, USHORT num,
                                struct RetMsg_t *ret) {
  eMBMasterReqErrCode errorCode = MB_MRE_NO_ERR;

  if (modbus_lock_take(Lock_Timeout) == false) {
    errorCode = MB_MRE_MASTER_BUSY;
    ESP_LOGE(TAG, "%4d %s eMBsend get lock timeout", __LINE__, __func__);
    return errorCode;
  }

  errorCode = eMBMasterAirswitchReq02(slaveAddr, target, num, 1);
  if (errorCode != MB_MRE_NO_ERR) {
    ESP_LOGE(TAG, "%4d %s eMBsend error", __LINE__, __func__);
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
static eMBMasterReqErrCode
modbus_airswitch001_sync_cmd_03(UCHAR slaveAddr, UCHAR subCmd, UCHAR target,
                                USHORT num, struct RetMsg_t *ret) {
  eMBMasterReqErrCode errorCode = MB_MRE_NO_ERR;

  if (modbus_lock_take(Lock_Timeout) == false) {
    errorCode = MB_MRE_MASTER_BUSY;
    ESP_LOGE(TAG, "%4d eMBsend get lock timeout", __LINE__);
    return errorCode;
  }

  errorCode = eMBMasterAirswitchReq03(slaveAddr, subCmd, target, num, 1);
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
static eMBMasterReqErrCode
modbus_airswitch001_sync_cmd_04(UCHAR slaveAddr, UCHAR subCmd, UCHAR target,
                                USHORT num, struct RetMsg_t *ret) {
  eMBMasterReqErrCode errorCode = MB_MRE_NO_ERR;

  if (modbus_lock_take(Lock_Timeout) == false) {
    errorCode = MB_MRE_MASTER_BUSY;
    ESP_LOGE(TAG, "%4d eMBsend get lock timeout", __LINE__);
    return errorCode;
  }

  errorCode = eMBMasterAirswitchReq04(slaveAddr, subCmd, target, num, 1);
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

/*
// sync cmd 05
static eMBMasterReqErrCode modbus_airswitch001_sync_cmd_05(
    UCHAR slaveAddr, USHORT target, USHORT num, struct RetMsg_t *ret)
{
  eMBMasterReqErrCode errorCode = MB_MRE_NO_ERR;

  if (modbus_lock_take(Lock_Timeout) == false)
  {
    errorCode = MB_MRE_MASTER_BUSY;
    ESP_LOGE(TAG, "%4d eMBsend get lock timeout", __LINE__);
    return errorCode;
  }

  errorCode = eMBMasterAirswitchReq05(slaveAddr, target, num, 1);
  if (errorCode != MB_MRE_NO_ERR)
  {
    ESP_LOGE(TAG, "%4d eMBsend error", __LINE__);
    goto EXIT;
  }

  ret->recvCmd = RetMsg->recvCmd;
  ret->retLen = RetMsg->retLen;
  memcpy(ret->retBuf, RetMsg->retBuf, RetMsg->retLen);

EXIT:
  modbus_lock_release();
  return errorCode;
}*/

// sync cmd 06
static eMBMasterReqErrCode
modbus_airswitch001_sync_cmd_06(UCHAR slaveAddr, UCHAR subCmd, UCHAR target,
                                USHORT num, struct RetMsg_t *ret) {
  eMBMasterReqErrCode errorCode = MB_MRE_NO_ERR;

  if (modbus_lock_take(Lock_Timeout) == false) {
    errorCode = MB_MRE_MASTER_BUSY;
    ESP_LOGE(TAG, "%4d eMBsend get lock timeout", __LINE__);
    return errorCode;
  }

  errorCode = eMBMasterAirswitchReq06(slaveAddr, subCmd, target, num, 1);
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

esp_err_t mt_modbus_airswitch001_task(int tx_pin, int rx_pin, int en_pin) {
  esp_err_t err = ESP_OK;

  err = modbus_airswitch001_init(2, 19200, 0, tx_pin, rx_pin, en_pin);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d modbus_init failed", __LINE__);
    goto EXIT;
  }

  xTaskCreate(modbus_loop, "mt_modbus_task", 1024 * 8, NULL, 12, NULL);

  // cache task
  cache_task();

EXIT:
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_modbus_airswitch001_task failed", __LINE__,
             __func__);
  }
  return err;
}

esp_err_t mt_airswitch001_get_addrs(UCHAR slaveAddr, USHORT *addrs) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // cmd process
  err = modbus_airswitch001_sync_cmd_01(slaveAddr, 0, 0xFF, &cmd_ret_payload);

  // ret check
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, slaveAddr);
    return err;
  }

  if (cmd_ret_payload.recvCmd != CMD_01_STAT_READ) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
             slaveAddr, cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != 2) {
    ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
             slaveAddr, cmd_ret_payload.retLen);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // data process
  *addrs = (int)cmd_ret_payload.retBuf[0] | (int)cmd_ret_payload.retBuf[1] << 8;
  ESP_LOGI(TAG, "%4d %s get addrs:%d", __LINE__, __func__, *addrs);

  return ESP_OK;
}

esp_err_t mt_airswitch001_get_state(UCHAR slaveAddr, USHORT target,
                                    bool *state) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;
  UCHAR real_target = 0;

  printf("debug2-1 free=%d", esp_get_free_heap_size());
  err = cache_get_target(target, &real_target);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s cache_get_target failed", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }
  printf("debug2-2 free=%d", esp_get_free_heap_size());

  // cmd process
  err = modbus_airswitch001_sync_cmd_01(slaveAddr, real_target, 1,
                                        &cmd_ret_payload);
  printf("debug2-3 free=%d", esp_get_free_heap_size());
  // ret check
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d target:%d real_target:%d failed", __LINE__,
             __func__, slaveAddr, target, real_target);
    return err;
  }

  if (cmd_ret_payload.recvCmd != CMD_01_STAT_READ) {
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d get error ret cmd:%d",
             __LINE__, __func__, slaveAddr, real_target,
             cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != 1) {
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d response process failed",
             __LINE__, __func__, slaveAddr, real_target);
    return false;
  }

  // data process
  if (cmd_ret_payload.retBuf[0] == 0 || cmd_ret_payload.retBuf[0] == 1) {
    if (cmd_ret_payload.retBuf[0] == 0) {
      *state = false;
    } else {
      *state = true;
    }
  } else {
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d response get error value:%d",
             __LINE__, __func__, slaveAddr, real_target,
             cmd_ret_payload.retBuf[0]);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

esp_err_t mt_airswitch001_get_ctrl(UCHAR slaveAddr, USHORT target, bool *ctrl) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;
  UCHAR real_target = 0;

  err = cache_get_target(target, &real_target);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s cache_get_target failed", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  // cmd process
  err = modbus_airswitch001_sync_cmd_02(slaveAddr, real_target, 1,
                                        &cmd_ret_payload);

  // ret check
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d failed", __LINE__, __func__,
             slaveAddr, real_target);
    return err;
  }

  if (cmd_ret_payload.recvCmd != CMD_02_CRTL_READ) {
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d get error ret cmd:%d",
             __LINE__, __func__, slaveAddr, real_target,
             cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != 1) {
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d response process failed",
             __LINE__, __func__, slaveAddr, real_target);
    return false;
  }

  // data process
  if (cmd_ret_payload.retBuf[0] == 0 || cmd_ret_payload.retBuf[0] == 1) {
    if (cmd_ret_payload.retBuf[0] == 0) {
      *ctrl = true;
    } else {
      *ctrl = false;
    }
  } else {
    ESP_LOGE(TAG, "%4d %s response get error value:%d", __LINE__, __func__,
             cmd_ret_payload.retBuf[0]);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

esp_err_t mt_airswitch001_get_data(UCHAR slaveAddr,
                                   Airswitch_Data_Read_Type type, UCHAR target,
                                   USHORT *value_out) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;
  UCHAR real_target = 0;

  err = cache_get_target(target, &real_target);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s cache_get_target:%d failed", __LINE__, __func__,
             target);
    return ESP_ERR_INVALID_ARG;
  }

  // cmd process
  switch (type) {
  case DATA_VOTAGE:
    break;
  case DATA_LEAK_CURRENT:
    break;
  case DATA_POWER:
    break;
  case DATA_TEMP:
    break;
  case DATA_CURRENT:
    break;
  case DATA_WARN:
    break;
  case DATA_QUALITY_L:
    break;
  case DATA_QUALITY_H:
    break;
  default:
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d get unexcept type:%d",
             __LINE__, __func__, slaveAddr, real_target, type);
    return ESP_ERR_INVALID_ARG;
    break;
  }
  err = modbus_airswitch001_sync_cmd_03(slaveAddr, type, real_target, 1,
                                        &cmd_ret_payload);

  // ret check
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d failed", __LINE__, __func__,
             slaveAddr, real_target);
    return err;
  }

  if (cmd_ret_payload.recvCmd != CMD_03_DATA_READ) {
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d get error ret cmd:%d",
             __LINE__, __func__, slaveAddr, real_target,
             cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != 2) {
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d get error ret size:%d",
             __LINE__, __func__, slaveAddr, real_target,
             cmd_ret_payload.retLen);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // data process
  *value_out =
      (int)cmd_ret_payload.retBuf[0] << 8 | (int)cmd_ret_payload.retBuf[1];

  return ESP_OK;
}

esp_err_t mt_airswitch001_get_warn(UCHAR slaveAddr, UCHAR target,
                                   airswitch_warn_t *warn) {
  esp_err_t err = ESP_OK;
  USHORT warn_num = 0;

  err = mt_airswitch001_get_data(slaveAddr, DATA_WARN, target, &warn_num);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d target:%d state failed code:%d", __LINE__,
             __func__, slaveAddr, target, err);
    return ESP_ERR_INVALID_RESPONSE;
  }

  warn->note_current = (warn_num & (((USHORT)1) << 15) >> 15);
  warn->note_leak = (warn_num & (((USHORT)1) << 14) >> 14);
  warn->note_votage_low = (warn_num & (((USHORT)1) << 13) >> 13);
  warn->note_votage_high = (warn_num & (((USHORT)1) << 12) >> 12);
  warn->warn_votage_low = (warn_num & (((USHORT)1) << 11) >> 11);
  warn->warn_fire = (warn_num & (((USHORT)1) << 10) >> 10);
  warn->warn_votage_high = (warn_num & (((USHORT)1) << 6) >> 6);
  warn->warn_current = (warn_num & (((USHORT)1) << 5) >> 5);
  warn->warn_leak = (warn_num & (((USHORT)1) << 4) >> 4);
  warn->warn_temp = (warn_num & (((USHORT)1) << 3) >> 3);
  warn->warn_power = (warn_num & (((USHORT)1) << 2) >> 2);
  warn->warn_wave = (warn_num & (((USHORT)1) << 1) >> 1);
  warn->warn_short = (warn_num & (((USHORT)1) << 0) >> 0);

  return ESP_OK;
}

esp_err_t mt_airswitch001_get_config(UCHAR slaveAddr,
                                     Airswitch_Cfg_Read_Type type, UCHAR target,
                                     USHORT *value_out) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;
  UCHAR real_target = 0;

  err = cache_get_target(target, &real_target);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s cache_get_target failed", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  // cmd process
  switch (type) {
  case CFG_READ_VOTAGE_HIGH:
    break;
  case CFG_READ_VOTAGE_LOW:
    break;
  case CFG_READ_LEAK_CURRENT:
    break;
  case CFG_READ_POWER_HIGH:
    break;
  case CFG_READ_TEMP_HIGH:
    break;
  case CFG_READ_CURRENT_HIGH:
    break;
  case CFG_READ_MODEL:
    break;
  default:
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d get unexcept type:%d",
             __LINE__, __func__, slaveAddr, real_target, type);
    return ESP_ERR_INVALID_ARG;
    break;
  }
  err = modbus_airswitch001_sync_cmd_04(slaveAddr, type, real_target, 1,
                                        &cmd_ret_payload);

  // ret check
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d failed", __LINE__, __func__,
             slaveAddr, real_target);
    return err;
  }

  if (cmd_ret_payload.recvCmd != CMD_04_PARM_READ) {
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d get error ret cmd:%d",
             __LINE__, __func__, real_target, slaveAddr,
             cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != 2) {
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d get error ret size:%d",
             __LINE__, __func__, slaveAddr, real_target,
             cmd_ret_payload.retLen);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // data process
  *value_out =
      (int)cmd_ret_payload.retBuf[0] << 8 | (int)cmd_ret_payload.retBuf[1];

  return ESP_OK;
}

esp_err_t mt_airswitch001_set_state(UCHAR slaveAddr, USHORT target,
                                    bool state) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;
  UCHAR real_target = 0;

  err = cache_get_target(target, &real_target);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s cache_get_target failed", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  // cmd process
  if (state == true) {
    err = modbus_airswitch001_sync_cmd_06(slaveAddr, CFG_WRITE_SET_STAT,
                                          real_target, 0x5A, &cmd_ret_payload);
  } else {
    err = modbus_airswitch001_sync_cmd_06(slaveAddr, CFG_WRITE_SET_STAT,
                                          real_target, 0xA5, &cmd_ret_payload);
  }

  // ret check
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d failed", __LINE__, __func__,
             slaveAddr, real_target);
    return err;
  }

  if (cmd_ret_payload.recvCmd != CMD_06_PARM_WRITE) {
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d get error ret cmd:%d",
             __LINE__, __func__, slaveAddr, real_target,
             cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != 2) {
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d get error ret size:%d",
             __LINE__, __func__, slaveAddr, real_target,
             cmd_ret_payload.retLen);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

esp_err_t mt_airswitch001_set_config(UCHAR slaveAddr,
                                     Airswitch_Cfg_Write_Type type,
                                     USHORT target, USHORT value) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;
  UCHAR real_target = 0;

  err = cache_get_target(target, &real_target);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s cache_get_target failed", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  // cmd process
  switch (type) {
  case CFG_WRITE_VOTAGE_HIGH:
    break;
  case CFG_WRITE_VOTAGE_LOW:
    break;
  case CFG_WRITE_LEAK_CURRENT_HIGH:
    break;
  case CFG_WRITE_POWER_HIGH:
    break;
  case CFG_WRITE_TEMP_HIGH:
    break;
  case CFG_WRITE_CURRENT_HIGH:
    break;
  case CFG_WRITE_SET_VOTAGE:
    break;
  case CFG_WRITE_SET_STAT:
    break;
  case CFG_WRITE_LEAK_TEST:
    value = 0x5A;
    break;
  case CFG_WRITE_WARN_VOTAGE_HIGH:
    break;
  case CFG_WRITE_WARN_VOTAGE_LOW:
    break;
  case CFG_WRITE_QUALITY_H:
    break;
  case CFG_WRITE_QUALITY_L:
    break;
  default:
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d get unexcept type:%d",
             __LINE__, __func__, slaveAddr, real_target, type);
    return ESP_ERR_INVALID_ARG;
    break;
  }

  err = modbus_airswitch001_sync_cmd_06(slaveAddr, type, real_target, value,
                                        &cmd_ret_payload);

  // ret check
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d failed", __LINE__, __func__,
             slaveAddr, real_target);
    return err;
  }

  if (cmd_ret_payload.recvCmd != CMD_06_PARM_WRITE) {
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d get error ret cmd:%d",
             __LINE__, __func__, slaveAddr, real_target,
             cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != 2) {
    ESP_LOGE(TAG, "%4d %s addr:%d real_target:%d get error ret size:%d",
             __LINE__, __func__, slaveAddr, real_target,
             cmd_ret_payload.retLen);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

esp_err_t mt_airswitch001_get_datas(UCHAR slaveAddr, UCHAR target, bool *state,
                                    bool *ctrl, double *votage,
                                    double *leak_current, double *power,
                                    double *temp, double *current) {
  esp_err_t err = ESP_OK;
  USHORT value = 0;
  bool bool_value = false;

  // state
  printf("debug1-1 free=%d", esp_get_free_heap_size());
  err = mt_airswitch001_get_state(slaveAddr, target, &bool_value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d target:%d state failed code:%d", __LINE__,
             __func__, slaveAddr, target, err);
    return ESP_ERR_INVALID_RESPONSE;
  }
  *state = bool_value;
  printf("debug1-2 free=%d", esp_get_free_heap_size());

  // ctrl
  err = mt_airswitch001_get_ctrl(slaveAddr, target, &bool_value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d target:%d ctrl failed code:%d", __LINE__,
             __func__, slaveAddr, target, err);
    return ESP_ERR_INVALID_RESPONSE;
  }
  *ctrl = bool_value;

  // DATA_VOTAGE
  err = mt_airswitch001_get_data(slaveAddr, DATA_VOTAGE, target, &value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d target:%d failed code:%d", __LINE__, __func__,
             slaveAddr, target, err);
    return ESP_ERR_INVALID_RESPONSE;
  }
  *votage = value;

  // DATA_LEAK_CURRENT
  err = mt_airswitch001_get_data(slaveAddr, DATA_LEAK_CURRENT, target, &value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d target:%d failed code:%d", __LINE__, __func__,
             slaveAddr, target, err);
    return ESP_ERR_INVALID_RESPONSE;
  }
  *leak_current = value / 10.0;

  // DATA_POWER
  err = mt_airswitch001_get_data(slaveAddr, DATA_POWER, target, &value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d target:%d failed code:%d", __LINE__, __func__,
             slaveAddr, target, err);
    return ESP_ERR_INVALID_RESPONSE;
  }
  *power = value / 10.0;

  // DATA_TEMP
  err = mt_airswitch001_get_data(slaveAddr, DATA_TEMP, target, &value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d target:%d failed code:%d", __LINE__, __func__,
             slaveAddr, target, err);
    return ESP_ERR_INVALID_RESPONSE;
  }
  *temp = value / 10.0;

  // DATA_CURRENT
  err = mt_airswitch001_get_data(slaveAddr, DATA_CURRENT, target, &value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d target:%d failed code:%d", __LINE__, __func__,
             slaveAddr, target, err);
    return ESP_ERR_INVALID_RESPONSE;
  }
  *current = value / 100.0;

  return ESP_OK;
}

esp_err_t mt_airswitch001_get_configs(UCHAR slaveAddr, UCHAR target,
                                      double *votage_high, double *votage_low,
                                      double *leak_current, double *power_high,
                                      double *temp_high, double *current_high) {
  USHORT value = 0;
  esp_err_t err = ESP_OK;

  // votage high
  err = mt_airswitch001_get_config(slaveAddr, CFG_READ_VOTAGE_HIGH, target,
                                   &value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d target:%d config failed code:%d", __LINE__,
             __func__, slaveAddr, target, err);
    return ESP_ERR_INVALID_RESPONSE;
  }
  *votage_high = (double)value;

  // votage low
  err = mt_airswitch001_get_config(slaveAddr, CFG_READ_VOTAGE_LOW, target,
                                   &value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d target:%d config failed code:%d", __LINE__,
             __func__, slaveAddr, target, err);
    return ESP_ERR_INVALID_RESPONSE;
  }
  *votage_low = (double)value;

  // leak current
  err = mt_airswitch001_get_config(slaveAddr, CFG_READ_LEAK_CURRENT, target,
                                   &value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d target:%d config failed code:%d", __LINE__,
             __func__, slaveAddr, target, err);
    return ESP_ERR_INVALID_RESPONSE;
  }
  *leak_current = (double)value / 10.0;

  // power high
  err = mt_airswitch001_get_config(slaveAddr, CFG_READ_POWER_HIGH, target,
                                   &value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d target:%d config failed code:%d", __LINE__,
             __func__, slaveAddr, target, err);
    return ESP_ERR_INVALID_RESPONSE;
  }
  *power_high = (double)value / 1.0;

  // temp high
  err =
      mt_airswitch001_get_config(slaveAddr, CFG_READ_TEMP_HIGH, target, &value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d target:%d config failed code:%d", __LINE__,
             __func__, slaveAddr, target, err);
    return ESP_ERR_INVALID_RESPONSE;
  }
  *temp_high = (double)value / 10.0;

  // current high
  err = mt_airswitch001_get_config(slaveAddr, CFG_READ_CURRENT_HIGH, target,
                                   &value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d target:%d config failed code:%d", __LINE__,
             __func__, slaveAddr, target, err);
    return ESP_ERR_INVALID_RESPONSE;
  }
  *current_high = (double)value / 100.0;

  return ESP_OK;
}

esp_err_t mt_airswitch001_set_configs(UCHAR slaveAddr, UCHAR target,
                                      double *votage_high, double *votage_low,
                                      double *leak_current_high,
                                      double *power_high, double *temp_high,
                                      double *current_high) {
  esp_err_t err = ESP_OK;

  // votage high
  if (votage_high != NULL) {
    err = mt_airswitch001_set_config(slaveAddr, CFG_WRITE_VOTAGE_HIGH, target,
                                     (int)(*votage_high));
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s set CFG_WRITE_VOTAGE_HIGH failed", __LINE__,
               __func__);
      return ESP_ERR_INVALID_ARG;
    }
  }

  // votage low
  if (votage_low != NULL) {
    err = mt_airswitch001_set_config(slaveAddr, CFG_WRITE_VOTAGE_LOW, target,
                                     (int)(*votage_low));
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s set CFG_WRITE_VOTAGE_LOW failed", __LINE__,
               __func__);
      return ESP_ERR_INVALID_ARG;
    }
  }

  // leak current
  if (leak_current_high != NULL) {
    err = mt_airswitch001_set_config(slaveAddr, CFG_WRITE_LEAK_CURRENT_HIGH,
                                     target, (int)(*leak_current_high * 10));
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s set CFG_WRITE_LEAK_CURRENT_HIGH failed", __LINE__,
               __func__);
      return ESP_ERR_INVALID_ARG;
    }
  }

  // power high
  if (power_high != NULL) {
    err = mt_airswitch001_set_config(slaveAddr, CFG_WRITE_POWER_HIGH, target,
                                     (int)(*power_high));
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s set CFG_WRITE_POWER_HIGH failed", __LINE__,
               __func__);
      return ESP_ERR_INVALID_ARG;
    }
  }

  // temp current
  if (temp_high != NULL) {
    err = mt_airswitch001_set_config(slaveAddr, CFG_WRITE_TEMP_HIGH, target,
                                     (int)(*temp_high * 10));
    printf("debug origin:%f new:%d", *temp_high, (int)(*temp_high * 10));
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s set CFG_WRITE_TEMP_HIGH failed", __LINE__,
               __func__);
      return ESP_ERR_INVALID_ARG;
    }
  }

  // current
  if (current_high != NULL) {
    err = mt_airswitch001_set_config(slaveAddr, CFG_WRITE_CURRENT_HIGH, target,
                                     (int)(*current_high * 100));
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s set CFG_WRITE_CURRENT_HIGH failed", __LINE__,
               __func__);
      return ESP_ERR_INVALID_ARG;
    }
  }

  return ESP_OK;
}

esp_err_t mt_airswitch001_get_model(UCHAR slaveAddr, UCHAR target, int *model,
                                    int *current) {
  esp_err_t err = ESP_OK;
  USHORT value = 0;

  err = mt_airswitch001_get_config(slaveAddr, CFG_READ_MODEL, target, &value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d target:%d failed code:%d", __LINE__, __func__,
             slaveAddr, target, err);
    return ESP_ERR_INVALID_RESPONSE;
  }

  *model = value >> 8;
  *current = value & 0x00FF;

  return ESP_OK;
}

esp_err_t mt_airswitch001_get_model_no_cache(UCHAR slaveAddr, UCHAR target,
                                             int *model, int *current) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  err = modbus_airswitch001_sync_cmd_04(slaveAddr, CFG_READ_MODEL, target, 1,
                                        &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d target:%d failed code:%d", __LINE__, __func__,
             slaveAddr, target, err);
    return ESP_ERR_INVALID_RESPONSE;
  }

  *model = (int)cmd_ret_payload.retBuf[0];
  *current = (int)cmd_ret_payload.retBuf[1];

  return ESP_OK;
}

esp_err_t mt_airswitch001_get_cache_quality(UCHAR slaveAddr, UCHAR target,
                                            double *quality) {
  esp_err_t err = ESP_OK;
  int32_t qualit_int = 0;
  int32_t qualit_save = 0;

  err = cache_quality_get(target, &qualit_int, &qualit_save);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s cache_quality_get failed", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  *quality = qualit_int / 1000.0;

  return ESP_OK;
}

esp_err_t mt_airswitch001_set_cache_quality(UCHAR slaveAddr, UCHAR target,
                                            double quality) {
  esp_err_t err = ESP_OK;

  err = cache_quality_set(target, (int32_t)(quality * 1000));
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s cache_quality_set failed", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}

esp_err_t mt_airswitch001_set_leak_test(UCHAR slaveAddr, UCHAR target) {
  esp_err_t err = ESP_OK;
  USHORT value = 0;

  err =
      mt_airswitch001_set_config(slaveAddr, CFG_WRITE_LEAK_TEST, target, value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_airswitch001_set_config failed", __LINE__,
             __func__);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}
