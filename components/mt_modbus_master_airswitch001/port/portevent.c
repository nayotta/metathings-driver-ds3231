/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portevent.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "esp_log.h"

#include "mt_mb_airswitch001.h"
#include "mt_mbconfig_airswitch001.h"
#include "mt_mbport_airswitch001.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

/* ----------------------- Variables ----------------------------------------*/
static const char *TAG = "MT_MODBUS_MASTER_PORTEVENT";

static xQueueHandle xQueueMasterHdl;    // for mb.c
static xQueueHandle xQueueMasterResHdl; // for response match
static SemaphoreHandle_t SemaphorMasterHdl = NULL;

BOOL xMBMasterAirswitchPortEventInit(void) {
  BOOL bStatus = FALSE;
  if (0 != (xQueueMasterHdl = xQueueCreate(1, sizeof(eMBMasterEventType)))) {
    bStatus = TRUE;
  }
  return bStatus;
}

BOOL xMBMasterAirswitchPortEventPost(eMBMasterEventType eEvent) {
  BOOL bStatus = TRUE;
  if (bMBPortIsWithinException()) {
    (void)xQueueSendFromISR(xQueueMasterHdl, (const void *)&eEvent, pdFALSE);
  } else {
    (void)xQueueSend(xQueueMasterHdl, (const void *)&eEvent, pdFALSE);
  }

  return bStatus;
}

BOOL xMBMasterAirswitchPortEventGet(eMBMasterEventType *eEvent) {
  BOOL xEventHappened = FALSE;
  if (pdTRUE == xQueueReceive(xQueueMasterHdl, eEvent, portTICK_RATE_MS * 10)) {
    xEventHappened = TRUE;
  }
  return xEventHappened;
}

BOOL xMBMasterAirswitchResEventInit(void) {
  BOOL bStatus = FALSE;
  if (0 !=
      (xQueueMasterResHdl = xQueueCreate(1, sizeof(eMBMasterResEventType)))) {
    bStatus = TRUE;
  }
  return bStatus;
}

BOOL xMBMasterAirswitchResEventClean(void) {
  xQueueReset(xQueueMasterResHdl);

  return TRUE;
}

BOOL xMBMasterAirswitchResEventPost(eMBMasterResEventType eEvent) {
  BOOL bStatus = TRUE;
  if (bMBPortIsWithinException()) {
    (void)xQueueSendFromISR(xQueueMasterResHdl, (const void *)&eEvent, pdFALSE);
  } else {
    (void)xQueueSend(xQueueMasterResHdl, (const void *)&eEvent, pdFALSE);
  }

  return bStatus;
}

BOOL xMBMasterAirswitchResEventGet(eMBMasterResEventType *eEvent) {
  BOOL xEventHappened = FALSE;
  if (pdTRUE ==
      xQueueReceive(xQueueMasterResHdl, eEvent, portTICK_RATE_MS * 10)) {
    xEventHappened = TRUE;
  }
  return xEventHappened;
}

void vMBMasterAirswitchResErrorCBRespondTimeout() {
  BOOL ret = xMBMasterAirswitchResEventPost(EV_RES_ERROR_RESPOND_TIMEOUT);
  if (!ret)
    ESP_LOGE(TAG, "%4d EV_RES_ERROR_RESPOND_TIMEOUT", __LINE__);
}

void vMBMasterAirswitchResCBRequestScuuess(void) {
  BOOL ret = xMBMasterAirswitchResEventPost(EV_RES_PROCESS_SUCESS);
  if (!ret)
    ESP_LOGI(TAG, "%4d EV_RES_PROCESS_SUCESS", __LINE__);
}

void vMBMasterAirswitchResErrorCBReceiveData() {
  BOOL ret = xMBMasterAirswitchResEventPost(EV_RES_ERROR_RECEIVE_DATA);
  if (!ret)
    ESP_LOGE(TAG, "%4d EV_RES_ERROR_RECEIVE_DATA", __LINE__);
}

void vMBMasterAirswitchResErrorCBExecuteFunction() {
  BOOL ret = xMBMasterAirswitchResEventPost(EV_RES_ERROR_EXECUTE_FUNCTION);
  if (!ret)
    ESP_LOGE(TAG, "%4d EV_RES_ERROR_EXECUTE_FUNCTION", __LINE__);
}

eMBMasterReqErrCode eMBMasterAirswitchWaitRequestFinish(void) {
  eMBMasterReqErrCode eErrStatus = MB_MRE_NO_ERR;
  eMBMasterResEventType recvedEvent;
RESTART:
  if (xMBMasterAirswitchResEventGet(&recvedEvent) == false) {
    goto RESTART;
  }
  switch (recvedEvent) {
  case EV_RES_PROCESS_SUCESS:
    break;
  case EV_RES_ERROR_RESPOND_TIMEOUT: {
    eErrStatus = MB_MRE_TIMEDOUT;
    ESP_LOGE(TAG, "%4d EV_RES_ERROR_RESPOND_TIMEOUT", __LINE__);
    break;
  }
  case EV_RES_ERROR_RECEIVE_DATA: {
    eErrStatus = MB_MRE_REV_DATA;
    ESP_LOGE(TAG, "%4d EV_RES_ERROR_RECEIVE_DATA", __LINE__);
    break;
  }
  case EV_RES_ERROR_EXECUTE_FUNCTION: {
    eErrStatus = MB_MRE_EXE_FUN;
    ESP_LOGE(TAG, "%4d EV_RES_ERROR_EXECUTE_FUNCTION", __LINE__);
    break;
  }
  default:
    break;
  }
  return eErrStatus;
}

BOOL xMBMasterAirswitchRunResTake(LONG lTimeOut) {
  /*If waiting time is -1 .It will wait forever */
  if (xSemaphoreTake(SemaphorMasterHdl, (portTickType)lTimeOut) ==
      pdTRUE) // portMAX_DELAY
  {
    return TRUE;
  }

  return FALSE;
}

void vMBMasterAirswitchRunResRelease(void) {
  xSemaphoreGive(SemaphorMasterHdl);
  return;
}

void vMBMasterAirswitchOsResInit(void) {
  SemaphorMasterHdl = xSemaphoreCreateMutex();
  if (SemaphorMasterHdl == NULL) {
    ESP_LOGE(TAG, "%4d %s xSemaphoreCreateMutex failed", __LINE__, __func__);
  }
}
