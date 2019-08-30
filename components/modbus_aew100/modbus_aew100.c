#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "mt_mbfunc.h"
#include "mt_port.h"

#include "modbus_aew100.h"

// global define ==============================================================
static const char *TAG = "MT_MODBUS_AEW100";
static SemaphoreHandle_t SemaphorMasterHdl = NULL;
static int Lock_Timeout = 50;

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

// static func ================================================================
static void modbus_lock_init()
{
  SemaphorMasterHdl = xSemaphoreCreateMutex();
  return;
}

static bool modbus_lock_take(LONG timeout)
{
  if (xSemaphoreTake(SemaphorMasterHdl, (portTickType)timeout) == pdTRUE)
  {
    return true;
  }
  return false;
}

static void modbus_lock_release()
{
  xSemaphoreGive(SemaphorMasterHdl);
  return;
}

// global func ================================================================
// cmd 01 callback
eMBErrorCode eMBMasterCB01(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen)
{
  // unuse this func
  return MB_EILLSTATE;
}

// cmd 02 callback
eMBErrorCode eMBMasterCB02(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen)
{
  // unuse this func
  return MB_EILLSTATE;
}

// cmd 03 callback
eMBErrorCode eMBMasterCB03(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen)
{
  eMBErrorCode eStatus = MB_ENOERR;

  if (recvLen > BUF_MAXLEN)
  {
    return MB_EILLSTATE;
  }

  memcpy(RetMsg->retBuf, recvBuf, recvLen);
  RetMsg->recvCmd = recvCmd;
  RetMsg->retLen = recvLen;

  return eStatus;
}

// cmd 04 callback
eMBErrorCode eMBMasterCB04(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen)
{
  eMBErrorCode eStatus = MB_ENOERR;

  if (recvLen > BUF_MAXLEN)
  {
    return MB_EILLSTATE;
  }

  memcpy(RetMsg->retBuf, recvBuf, recvLen);
  RetMsg->recvCmd = recvCmd;
  RetMsg->retLen = recvLen;

  return eStatus;
}

// cmd 05 callback
eMBErrorCode eMBMasterCB05(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen)
{
  // unuse this func
  return MB_EILLSTATE;
}

// cmd 06 callback
eMBErrorCode eMBMasterCB06(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen)
{
  // unuse this func
  return MB_EILLSTATE;
}

eMBErrorCode modbus_aew100_init(UCHAR ucPort, ULONG ulBaudRate,
                                eMBParity eParity, int tx_pin, int rx_pin,
                                int en_pin)
{
  eMBErrorCode ret = 0;

  // default 9600 8in1  config
  ret = eMBMasterInit(MB_RTU, ucPort, ulBaudRate, eParity, tx_pin, rx_pin,
                      en_pin);
  if (ret != 0)
  {
    ESP_LOGE(TAG, "%4d eMBInit failed!!! eStatus: %d", __LINE__, ret);
  }

  mt_vMBMaster_set_T35_interval(250); // T35 set 350ms

  return ret;
}

// modbus main loop
static void modbus_aew100_loop(void *parameter)
{
  eMBErrorCode eStatus;

  // theard lock
  modbus_lock_init();

  RetMsg = malloc(sizeof(struct RetMsg_t)); // global no need free
  memset(RetMsg->retBuf, 0, BUF_MAXLEN);
  RetMsg->recvCmd = 0;
  RetMsg->retLen = 0;

  // master enable
  ESP_LOGI(TAG, "%4d eMBInit OK.", __LINE__);
  eStatus = eMBMasterEnable();
  if (eStatus != 0)
  {
    ESP_LOGE(TAG, "%4d eMBEnable failed!!! eStatus: %d", __LINE__, eStatus);
  }

  // master loop
  ESP_LOGI(TAG, "%4d starting eMBMasterPoll...", __LINE__);
  while (1)
  {
    // (TODO zh) if need delay?
    eMBMasterPoll();
    vTaskDelay(1);
  }

  ESP_LOGE(TAG, "%4d eMBmasterPoll unexcept exit!!!", __LINE__);
  vTaskDelete(NULL);
}

// sync cmd 03
eMBMasterReqErrCode modbus_aew100_sync_cmd_03(UCHAR slaveAddr, USHORT target,
                                              USHORT num,
                                              struct RetMsg_t *ret)
{
  eMBMasterReqErrCode errorCode = MB_MRE_NO_ERR;

  if (modbus_lock_take(Lock_Timeout) == false)
  {
    errorCode = MB_MRE_MASTER_BUSY;
    ESP_LOGE(TAG, "%4d eMBsend get lock timeout", __LINE__);
    return errorCode;
  }

  errorCode = eMBMasterReq03(slaveAddr, target, num, 1);
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
}

void mt_modbus_aew100_task()
{
  xTaskCreate(modbus_aew100_loop, "mt_modbus_aew100_task", 1024 * 8, NULL, 8,
              NULL);
  vTaskDelay(2 * 1000 / portTICK_RATE_MS);
}

esp_err_t mt_aew100_get_currentA(UCHAR addr, double *current)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // current A
  err = modbus_aew100_sync_cmd_03(addr, DATA_CURRENTA, 1, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2)
    {
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

esp_err_t mt_aew100_get_currentB(UCHAR addr, double *current)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // current B
  err = modbus_aew100_sync_cmd_03(addr, DATA_CURRENTB, 1, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2)
    {
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

esp_err_t mt_aew100_get_currentC(UCHAR addr, double *current)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // current C
  err = modbus_aew100_sync_cmd_03(addr, DATA_CURRENTC, 1, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2)
    {
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

esp_err_t mt_aew100_get_votageA(UCHAR addr, double *data)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // votage A
  err = modbus_aew100_sync_cmd_03(addr, DATA_VOTAGEA, 1, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            10.0;
  }

  return ESP_OK;
}

esp_err_t mt_aew100_get_votageB(UCHAR addr, double *data)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // votage B
  err = modbus_aew100_sync_cmd_03(addr, DATA_VOTAGEB, 1, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            10.0;
  }

  return ESP_OK;
}

esp_err_t mt_aew100_get_votageC(UCHAR addr, double *data)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // votage C
  err = modbus_aew100_sync_cmd_03(addr, DATA_VOTAGEC, 1, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            10.0;
  }

  return ESP_OK;
}

esp_err_t mt_aew100_get_activePowerA(UCHAR addr, double *data)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // activePowerA
  err = modbus_aew100_sync_cmd_03(addr, DATA_ACTIVEPOWERA, 2, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 4)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = (cmd_ret_payload.retBuf[0] * (1 << 24) +
             cmd_ret_payload.retBuf[1] * (1 << 16) +
             cmd_ret_payload.retBuf[2] * (1 << 8) + cmd_ret_payload.retBuf[3]) /
            1000.0;
    if (cmd_ret_payload.retBuf[0] == 0xff)
    {
      *data = 0;
    }
  }

  return ESP_OK;
}

esp_err_t mt_aew100_get_activePowerB(UCHAR addr, double *data)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // activePowerB
  err = modbus_aew100_sync_cmd_03(addr, DATA_ACTIVEPOWERB, 2, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 4)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = (cmd_ret_payload.retBuf[0] * (1 << 24) +
             cmd_ret_payload.retBuf[1] * (1 << 16) +
             cmd_ret_payload.retBuf[2] * (1 << 8) + cmd_ret_payload.retBuf[3]) /
            1000.0;

    if (cmd_ret_payload.retBuf[0] == 0xff)
    {
      *data = 0;
    }
  }

  return ESP_OK;
}

esp_err_t mt_aew100_get_activePowerC(UCHAR addr, double *data)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // activePowerC
  err = modbus_aew100_sync_cmd_03(addr, DATA_ACTIVEPOWERC, 2, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 4)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = (cmd_ret_payload.retBuf[0] * (1 << 24) +
             cmd_ret_payload.retBuf[1] * (1 << 16) +
             cmd_ret_payload.retBuf[2] * (1 << 8) + cmd_ret_payload.retBuf[3]) /
            1000.0;

    if (cmd_ret_payload.retBuf[0] == 0xff)
    {
      *data = 0;
    }
  }

  return ESP_OK;
}

esp_err_t mt_aew100_get_reactivePowerA(UCHAR addr, double *data)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // reactivePowerA
  err =
      modbus_aew100_sync_cmd_03(addr, DATA_REACTIVEPOWERA, 2, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 4)
    {
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

esp_err_t mt_aew100_get_reactivePowerB(UCHAR addr, double *data)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // reactivePowerB
  err =
      modbus_aew100_sync_cmd_03(addr, DATA_REACTIVEPOWERB, 2, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 4)
    {
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

esp_err_t mt_aew100_get_reactivePowerC(UCHAR addr, double *data)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // reactivePowerC
  err =
      modbus_aew100_sync_cmd_03(addr, DATA_REACTIVEPOWERA, 2, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 4)
    {
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

esp_err_t mt_aew100_get_powerFactorA(UCHAR addr, double *data)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // powerFactorA
  err = modbus_aew100_sync_cmd_03(addr, DATA_POWERFACTORA, 1, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            1000.0;
  }

  return ESP_OK;
}

esp_err_t mt_aew100_get_powerFactorB(UCHAR addr, double *data)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // powerFactorB
  err = modbus_aew100_sync_cmd_03(addr, DATA_POWERFACTORB, 1, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            1000.0;
  }

  return ESP_OK;
}

esp_err_t mt_aew100_get_powerFactorC(UCHAR addr, double *data)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // powerFactorC
  err = modbus_aew100_sync_cmd_03(addr, DATA_POWERFACTORC, 1, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            1000.0;
  }

  return ESP_OK;
}

esp_err_t mt_aew100_get_qualityA(UCHAR addr, double *data)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // qualityA
  err = modbus_aew100_sync_cmd_03(addr, DATA_QUALITYA, 1, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            1000.0;
  }

  return ESP_OK;
}

esp_err_t mt_aew100_get_qualityB(UCHAR addr, double *data)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // qualityB
  err = modbus_aew100_sync_cmd_03(addr, DATA_QUALITYB, 1, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            1000.0;
  }

  return ESP_OK;
}

esp_err_t mt_aew100_get_qualityC(UCHAR addr, double *data)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // qualityC
  err = modbus_aew100_sync_cmd_03(addr, DATA_QUALITYC, 1, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            1000.0;
  }

  return ESP_OK;
}

esp_err_t mt_aew100_get_tempA(UCHAR addr, double *data)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // tempA
  err = modbus_aew100_sync_cmd_03(addr, DATA_TEMPA, 1, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            1000.0;
  }

  return ESP_OK;
}

esp_err_t mt_aew100_get_tempB(UCHAR addr, double *data)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // tempB
  err = modbus_aew100_sync_cmd_03(addr, DATA_TEMPB, 1, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            1000.0;
  }

  return ESP_OK;
}

esp_err_t mt_aew100_get_tempC(UCHAR addr, double *data)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // tempC
  err = modbus_aew100_sync_cmd_03(addr, DATA_TEMPC, 1, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != MODBUS_READ)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *data = (cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1]) /
            1000.0;
  }

  return ESP_OK;
}

esp_err_t mt_aew100_get_data(UCHAR addr, Aew100_data_t *data)
{
  esp_err_t err = ESP_OK;

  err = mt_aew100_get_currentA(addr, &data->currentA);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_currentA failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_currentB(addr, &data->currentB);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_currentB failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_currentC(addr, &data->currentC);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_currentC failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_votageA(addr, &data->votageA);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_votageA failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_votageB(addr, &data->votageB);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_votageB failed", __LINE__,
             __func__, addr);
    return err;
  }
  err = mt_aew100_get_votageC(addr, &data->votageC);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_votageC failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_activePowerA(addr, &data->activePowerA);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_activePowerA failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_activePowerB(addr, &data->activePowerB);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_activePowerB failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_activePowerC(addr, &data->activePowerC);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_activePowerC failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_reactivePowerA(addr, &data->reactivePowerA);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_reactivePowerA failed",
             __LINE__, __func__, addr);
    return err;
  }

  err = mt_aew100_get_reactivePowerB(addr, &data->reactivePowerB);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_reactivePowerB failed",
             __LINE__, __func__, addr);
    return err;
  }

  err = mt_aew100_get_reactivePowerC(addr, &data->reactivePowerC);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_reactivePowerC failed",
             __LINE__, __func__, addr);
    return err;
  }

  err = mt_aew100_get_powerFactorA(addr, &data->powerFactorA);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_powerFactorA failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_powerFactorB(addr, &data->powerFactorB);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_powerFactorB failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_powerFactorC(addr, &data->powerFactorC);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_powerFactorC failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_qualityA(addr, &data->qualityA);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_qualityA failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_qualityB(addr, &data->qualityB);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_qualityB failed", __LINE__,
             __func__, addr);
    return err;
  }

  err = mt_aew100_get_qualityC(addr, &data->qualityC);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d mt_aew100_get_qualityC failed", __LINE__,
             __func__, addr);
    return err;
  }

  return ESP_OK;
}
