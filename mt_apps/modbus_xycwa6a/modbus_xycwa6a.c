#include "esp_timer.h"
#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "mt_mbfunc.h"
#include "mt_mbport.h"
#include "mt_port.h"

#include "modbus_xycwa6a.h"

// global define ==============================================================
static const char *TAG = "MT_MODBUS_xycwa6a";
static SemaphoreHandle_t SemaphorMasterHdl = NULL;
static int Lock_Timeout = 50;

#define XYCWA6A_READ 03

#define XYCWA6A_CMD_TEMP1_ADDR 0x46D
#define XYCWA6A_CMD_TEMP2_ADDR 0x46E
#define XYCWA6A_CMD_TEMP3_ADDR 0x46F
#define XYCWA6A_CMD_TEMP4_ADDR 0x470
#define XYCWA6A_CMD_TEMP5_ADDR 0x471
#define XYCWA6A_CMD_TEMP6_ADDR 0x472

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
  // unuse this func
  return MB_EILLSTATE;
}

// cmd 02 callback
eMBErrorCode eMBMasterCB02(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen) {
  // unuse this func
  return MB_EILLSTATE;
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
  // unuse this func
  return MB_EILLSTATE;
}

// cmd 05 callback
eMBErrorCode eMBMasterCB05(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen) {
  // unuse this func
  return MB_EILLSTATE;
}

// cmd 06 callback
eMBErrorCode eMBMasterCB06(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen) {
  // unuse this func
  return MB_EILLSTATE;
}

eMBErrorCode modbus_xycwa6a_init(UCHAR ucPort, ULONG ulBaudRate,
                                 eMBParity eParity, int tx_pin, int rx_pin,
                                 int en_pin) {
  eMBErrorCode ret = 0;

  // default 9600 8in1  config
  ret = eMBMasterInit(MB_RTU, ucPort, ulBaudRate, eParity, tx_pin, rx_pin,
                      en_pin);
  if (ret != 0) {
    ESP_LOGE(TAG, "%4d eMBInit failed!!! eStatus: %d", __LINE__, ret);
  }

  mt_vMBMaster_set_T35_interval(350); // T35 set 350ms

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
  eStatus = eMBMasterEnable();
  if (eStatus != 0) {
    ESP_LOGE(TAG, "%4d eMBEnable failed!!! eStatus: %d", __LINE__, eStatus);
  }

  // master loop
  ESP_LOGI(TAG, "%4d starting eMBMasterPoll...", __LINE__);
  while (1) {
    // (TODO zh) if need delay?
    eMBMasterPoll();
    vTaskDelay(0 / portTICK_RATE_MS);
  }

  ESP_LOGE(TAG, "%4d eMBmasterPoll unexcept exit!!!", __LINE__);
  vTaskDelete(NULL);
}

// sync cmd 03
eMBMasterReqErrCode modbus_xycwa6a_sync_Cmd_03(UCHAR slaveAddr, USHORT target,
                                               USHORT num,
                                               struct RetMsg_t *ret) {
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

void mt_modbus_xycwa6a_task() {
  xTaskCreate(modbus_loop, "mt_modbus_task", 1024 * 8, NULL, 12, NULL);
}

esp_err_t mt_xycwa6a_get_temp(int addr, USHORT reg, double *temp) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  int64_t begin = esp_timer_get_time();
  ESP_LOGW(TAG, "begin time:%lld", begin / 1000);
  err = modbus_xycwa6a_sync_Cmd_03(addr, reg, 1, &cmd_ret_payload);
  ESP_LOGW(TAG, "end   time:%lld, interval:%lld", esp_timer_get_time() / 1000,
           (esp_timer_get_time() - begin) / 1000);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s addr:%d failed", __LINE__, __func__, addr);
    return err;
  } else {
    if (cmd_ret_payload.recvCmd != XYCWA6A_READ) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret cmd:%d", __LINE__, __func__,
               addr, cmd_ret_payload.recvCmd);
      return ESP_ERR_INVALID_RESPONSE;
    }

    if (cmd_ret_payload.retLen != 2) {
      ESP_LOGE(TAG, "%4d %s addr:%d get error ret size:%d", __LINE__, __func__,
               addr, cmd_ret_payload.retLen);
      return ESP_ERR_INVALID_RESPONSE;
    }

    *temp = (((uint32_t)(cmd_ret_payload.retBuf[0]) << 8) +
             cmd_ret_payload.retBuf[1]) /
            10.0;
    ESP_LOGW(TAG, "get temp:%f", *temp);
  }

  return ESP_OK;
}

esp_err_t mt_xycwa6a_get_temps(int addr, double *temp1, double *temp2,
                               double *temp3, double *temp4, double *temp5,
                               double *temp6) {
  esp_err_t err = ESP_OK;
  vTaskDelay(2000 / portTICK_RATE_MS);

  err = mt_xycwa6a_get_temp(addr, XYCWA6A_CMD_TEMP1_ADDR, temp1);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_xycwa6a_get_temp 1 failed", __LINE__, __func__);
    return err;
  }
  vTaskDelay(2000 / portTICK_RATE_MS);

  err = mt_xycwa6a_get_temp(addr, XYCWA6A_CMD_TEMP2_ADDR, temp2);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_xycwa6a_get_temp 2 failed", __LINE__, __func__);
    return err;
  }
  vTaskDelay(2000 / portTICK_RATE_MS);

  err = mt_xycwa6a_get_temp(addr, XYCWA6A_CMD_TEMP3_ADDR, temp3);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_xycwa6a_get_temp 3 failed", __LINE__, __func__);
    return err;
  }
  vTaskDelay(2000 / portTICK_RATE_MS);

  err = mt_xycwa6a_get_temp(addr, XYCWA6A_CMD_TEMP4_ADDR, temp4);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_xycwa6a_get_temp 4 failed", __LINE__, __func__);
    return err;
  }
  vTaskDelay(2000 / portTICK_RATE_MS);

  err = mt_xycwa6a_get_temp(addr, XYCWA6A_CMD_TEMP5_ADDR, temp5);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_xycwa6a_get_temp 5 failed", __LINE__, __func__);
    return err;
  }
  vTaskDelay(2000 / portTICK_RATE_MS);

  err = mt_xycwa6a_get_temp(addr, XYCWA6A_CMD_TEMP6_ADDR, temp6);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_xycwa6a_get_temp 6 failed", __LINE__, __func__);
    return err;
  }
  vTaskDelay(2000 / portTICK_RATE_MS);

  ESP_LOGI(TAG, "%4d %s temps:%3.1f | %3.1f | %3.1f |  %3.1f | %3.1f | %3.1f |",
           __LINE__, __func__, *temp1, *temp2, *temp3, *temp4, *temp5, *temp6);
  return ESP_OK;
}
