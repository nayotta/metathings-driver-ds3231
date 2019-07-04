#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "mt_mbfunc.h"
#include "mt_port.h"

#include "modbus_ld100.h"

// global define ==============================================================
static const char *TAG = "MT_MODBUS_LD100";
static SemaphoreHandle_t SemaphorMasterHdl = NULL;
static int Lock_Timeout = 50;

#define LD100_READ 04
#define LD100_WRITE 06

#define LD100_CMD_DEV_ADDR 00
#define LD100_CMD_DEV_STATE 01
#define LD100_CMD_DEV_RELAY 03

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
  // unuse this func
  return MB_EILLSTATE;
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

eMBErrorCode modbus_init(UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity,
                         int tx_pin, int rx_pin, int en_pin)
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
void modbus_loop(void *parameter)
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

// sync cmd 04
eMBMasterReqErrCode modbus_ld100_sync_Cmd_04(UCHAR slaveAddr, USHORT target,
                                             USHORT num, struct RetMsg_t *ret)
{
  eMBMasterReqErrCode errorCode = MB_MRE_NO_ERR;

  if (modbus_lock_take(Lock_Timeout) == false)
  {
    errorCode = MB_MRE_MASTER_BUSY;
    ESP_LOGE(TAG, "%4d eMBsend get lock timeout", __LINE__);
    return errorCode;
  }

  errorCode = eMBMasterReq04(slaveAddr, target, num, 1);
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

// sync cmd 06
eMBMasterReqErrCode modbus_ld100_sync_Cmd_06(UCHAR slaveAddr, USHORT target,
                                             USHORT num, struct RetMsg_t *ret)
{
  eMBMasterReqErrCode errorCode = MB_MRE_NO_ERR;

  if (modbus_lock_take(Lock_Timeout) == false)
  {
    errorCode = MB_MRE_MASTER_BUSY;
    ESP_LOGE(TAG, "%4d eMBsend get lock timeout", __LINE__);
    return errorCode;
  }

  errorCode = eMBMasterReq06(slaveAddr, target, num, 1);
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

void mt_modbus_task()
{
  xTaskCreate(modbus_loop, "mt_modbus_task", 1024 * 8, NULL, 8, NULL);
}

esp_err_t mt_ld100_get_addr(int addr, bool *state)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  err = modbus_ld100_sync_Cmd_04(addr, LD100_CMD_DEV_ADDR, 1, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    if (err == EV_RES_ERROR_RESPOND_TIMEOUT)
    {
      *state = false;
    }
    else
    {
      ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
      return err;
    }
  }
  else
  {
    if (cmd_ret_payload.recvCmd != LD100_READ)
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

    if (cmd_ret_payload.retBuf[0] != 0)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret buf 0:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retBuf[0]);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retBuf[1] != addr)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret buf 1:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retBuf[1]);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *state = true;
  }

  // ESP_LOGI(TAG, "%4d %s addr:%d state:%d", __LINE__, __func__, addr, *state);

  return ESP_OK;
}

esp_err_t mt_ld100_set_addr(int addr, int new_addr)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  err = modbus_ld100_sync_Cmd_06(addr, LD100_CMD_DEV_ADDR, new_addr,
                                 &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != LD100_WRITE)
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

    if (cmd_ret_payload.retBuf[0] != 0)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret buf 0:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retBuf[0]);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retBuf[1] != new_addr)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d new_addr:%d error ret buf 1:%d", __LINE__,
               __func__, addr, new_addr, cmd_ret_payload.retBuf[1]);
      return ESP_ERR_INVALID_RESPONSE;
    }
  }

  // ESP_LOGI(TAG, "%4d %s addr:%d new_addr:%d success", __LINE__, __func__,
  // addr, new_addr);

  return ESP_OK;
}

esp_err_t mt_ld100_get_state(int addr, bool *state)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  err =
      modbus_ld100_sync_Cmd_04(addr, LD100_CMD_DEV_STATE, 1, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != LD100_READ)
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

    if (cmd_ret_payload.retBuf[0] != 0)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret buf 0:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retBuf[0]);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retBuf[1] == 0 || cmd_ret_payload.retBuf[1] == 2)
    {
      if (cmd_ret_payload.retBuf[1] == 0)
        *state = true;

      if (cmd_ret_payload.retBuf[1] == 2)
        *state = false;
    }
    else
    {
      ESP_LOGE(TAG, "%4d %s addr:%d error ret buf 1:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retBuf[1]);
      return ESP_ERR_INVALID_RESPONSE;
    }
  }

  // ESP_LOGI(TAG, "%4d %s addr:%d success state:%d", __LINE__, __func__, addr,
  //*state);

  return ESP_OK;
}

esp_err_t mt_ld100_get_relay(int addr, bool *state)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  err =
      modbus_ld100_sync_Cmd_04(addr, LD100_CMD_DEV_RELAY, 1, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != LD100_READ)
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

    if (cmd_ret_payload.retBuf[0] != 0)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret buf 0:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retBuf[0]);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retBuf[1] == 0 || cmd_ret_payload.retBuf[1] == 1)
    {
      if (cmd_ret_payload.retBuf[1] == 0)
        *state = false;

      if (cmd_ret_payload.retBuf[1] == 1)
        *state = true;
    }
    else
    {
      ESP_LOGE(TAG, "%4d %s addr:%d error ret buf 1:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retBuf[1]);
      return ESP_ERR_INVALID_RESPONSE;
    }
  }

  // ESP_LOGI(TAG, "%4d %s addr:%d success state:%d", __LINE__, __func__, addr,
  //        *state);

  return ESP_OK;
}

esp_err_t mt_ld100_set_relay(int addr, bool state)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;
  USHORT target_state;

  if (state == true)
  {
    target_state = 1;
  }
  else
  {
    target_state = 0;
  }

  err = modbus_ld100_sync_Cmd_06(addr, LD100_CMD_DEV_RELAY, target_state,
                                 &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  }
  else
  {
    if (cmd_ret_payload.recvCmd != LD100_WRITE)
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

    if (cmd_ret_payload.retBuf[0] != 0)
    {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret buf 0:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retBuf[0]);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retBuf[1] == 0 || cmd_ret_payload.retBuf[1] == 1)
    {
      if (cmd_ret_payload.retBuf[1] != state)
      {
        ESP_LOGE(TAG, "%4d %s addr:%d target:%d ret:%d", __LINE__, __func__,
                 addr, state, cmd_ret_payload.retBuf[1]);
        return ESP_ERR_INVALID_RESPONSE;
      }
    }
    else
    {
      ESP_LOGE(TAG, "%4d %s addr:%d error ret buf 1:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retBuf[1]);
      return ESP_ERR_INVALID_RESPONSE;
    }
  }

  // ESP_LOGI(TAG, "%4d %s addr:%d success state:%d", __LINE__, __func__, addr,
  //         state);

  return ESP_OK;
}
