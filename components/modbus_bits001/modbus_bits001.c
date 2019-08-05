#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "mt_mbfunc.h"
#include "mt_port.h"

#include "modbus_bits001.h"

// global define ==============================================================
static const char *TAG = "MT_MODBUS_BITS001";
static SemaphoreHandle_t SemaphorMasterHdl = NULL;
static int Lock_Timeout = 50;

#define BITS001_READ 03

#define BITS001_CMD_TEMP_ADDR 01
#define BITS001_CMD_HUM_ADDR 02
#define BITS001_CMD_FROG_ADDR 11

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
  // unuse this func
  return MB_EILLSTATE;
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

eMBErrorCode modbus_bits001_init(UCHAR ucPort, ULONG ulBaudRate,
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
static void modbus_loop(void *parameter)
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
eMBMasterReqErrCode modbus_bits001_sync_Cmd_03(UCHAR slaveAddr, USHORT target,
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

void mt_modbus_bits001_task()
{
  xTaskCreate(modbus_loop, "mt_modbus_task", 1024 * 8, NULL, 8, NULL);
  vTaskDelay(2000 / portTICK_RATE_MS);
}

esp_err_t mt_bits001_get_temp(int addr, double *temp)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload = {"", 0, 0};

  err = modbus_bits001_sync_Cmd_03(addr, BITS001_CMD_TEMP_ADDR, 1,
                                   &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s temp:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != BITS001_READ)
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

    *temp = (((uint32_t)(cmd_ret_payload.retBuf[0]) << 8) +
             cmd_ret_payload.retBuf[1] - 2000) /
            100.0;
  }

  ESP_LOGI(TAG, "%4d %s addr:%d temp:%f", __LINE__, __func__, addr, *temp);

  return ESP_OK;
}

esp_err_t mt_bits001_get_hum(int addr, double *hum)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  err = modbus_bits001_sync_Cmd_03(addr, BITS001_CMD_HUM_ADDR, 1,
                                   &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s hum:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != BITS001_READ)
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

    *hum = (((uint32_t)(cmd_ret_payload.retBuf[0]) << 8) +
            cmd_ret_payload.retBuf[1]) /
           100.0;
  }

  ESP_LOGI(TAG, "%4d %s addr:%d hum:%f", __LINE__, __func__, addr, *hum);

  return ESP_OK;
}

esp_err_t mt_bits001_get_frog(int addr, double *frog)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  err = modbus_bits001_sync_Cmd_03(addr, BITS001_CMD_FROG_ADDR, 1,
                                   &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s frog:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != BITS001_READ)
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

    *frog = (((uint32_t)(cmd_ret_payload.retBuf[0]) << 8) +
             cmd_ret_payload.retBuf[1]);
  }

  ESP_LOGI(TAG, "%4d %s addr:%d frog:%f", __LINE__, __func__, addr, *frog);

  return ESP_OK;
}

esp_err_t mt_bits001_get_data(int addr, double *temp, double *hum, double *frog)
{
  esp_err_t err = ESP_OK;

  err = mt_bits001_get_temp(addr, temp);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mt_bits001_get_temp failed,code=%d", __LINE__, __func__, err);
    return err;
  }

  err = mt_bits001_get_hum(addr, hum);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mt_bits001_get_hum failed,code=%d", __LINE__, __func__, err);
    return err;
  }

  err = mt_bits001_get_frog(addr, frog);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mt_bits001_get_frog failed,code=%d", __LINE__, __func__, err);
    return err;
  }

  return ESP_OK;
}
