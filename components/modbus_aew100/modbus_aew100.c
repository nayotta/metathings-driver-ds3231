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
#define MODBUS_CURRENTA_ADDR 0x1A
#define MODBUS_CURRENTB_ADDR 0x1B
#define MODBUS_CURRENTC_ADDR 0x1C

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
  ESP_LOGW(TAG, "%4d %s debug", __LINE__, __func__);
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
  ESP_LOGW(TAG, "%4d %s debug", __LINE__, __func__);
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

  return ret;
}

// modbus main loop
static void modbus_aew100_loop(void *parameter)
{
  eMBErrorCode eStatus;

  // theard lock
  modbus_lock_init();

  RetMsg = malloc(sizeof(struct RetMsg_t)); // global no need free

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
  xTaskCreate(modbus_aew100_loop, "mt_modbus_aew100_task", 1024 * 8, NULL, 8, NULL);
  vTaskDelay(2 * 1000 / portTICK_RATE_MS);
}

esp_err_t mt_aew100_get_current(UCHAR addr, double *currentA, double *currentB,
                                double *currentC)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  // current A
  ESP_LOGW(TAG, "%4d %s get",__LINE__, __func__);
  err = modbus_aew100_sync_cmd_03(addr, MODBUS_CURRENTA_ADDR, 1, &cmd_ret_payload);
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

    *currentA =
        cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1];
  }

  // current B
  err = modbus_aew100_sync_cmd_03(addr, MODBUS_CURRENTB_ADDR, 1, &cmd_ret_payload);
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

    *currentB =
        cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1];
  }

  // current C
  err = modbus_aew100_sync_cmd_03(addr, MODBUS_CURRENTC_ADDR, 1, &cmd_ret_payload);
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

    *currentC =
        cmd_ret_payload.retBuf[0] * (1 << 8) + cmd_ret_payload.retBuf[1];
  }

  ESP_LOGI(TAG, "%4d %s get current process, A:%f, B:%f, C:%f", __LINE__,
           __func__, *currentA, *currentB, *currentC);

  return ESP_OK;
}
