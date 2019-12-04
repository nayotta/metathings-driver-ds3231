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

#include "mt_mb.h"
#include "mt_mbconfig.h"
#include "mt_mbport.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

/* ----------------------- Variables ----------------------------------------*/
static const char *TAG = "MT_MODBUS_MASTER_PORTEVENT";

static xQueueHandle xQueueMasterHdl;    // for mb.c
static xQueueHandle xQueueMasterResHdl; // for response match
static SemaphoreHandle_t SemaphorMasterHdl = NULL;
;

BOOL xMBMasterPortEventInit(void)
{
  BOOL bStatus = FALSE;
  if (0 != (xQueueMasterHdl = xQueueCreate(1, sizeof(eMBMasterEventType))))
  {
    bStatus = TRUE;
  }
  return bStatus;
}

BOOL xMBMasterPortEventPost(eMBMasterEventType eEvent)
{
  BOOL bStatus = TRUE;
  if (bMBPortIsWithinException())
  {
    (void)xQueueSendFromISR(xQueueMasterHdl, (const void *)&eEvent, pdFALSE);
  }
  else
  {
    (void)xQueueSend(xQueueMasterHdl, (const void *)&eEvent, pdFALSE);
  }

  return bStatus;
}

BOOL xMBMasterPortEventGet(eMBMasterEventType *eEvent)
{
  BOOL xEventHappened = FALSE;
  if (pdTRUE == xQueueReceive(xQueueMasterHdl, eEvent, portTICK_RATE_MS * 10))
  {
    xEventHappened = TRUE;
  }
  return xEventHappened;
}

BOOL xMBMasterResEventInit(void)
{
  BOOL bStatus = FALSE;
  if (0 !=
      (xQueueMasterResHdl = xQueueCreate(1, sizeof(eMBMasterResEventType))))
  {
    bStatus = TRUE;
  }
  return bStatus;
}

BOOL xMBMasterResEventClean(void)
{
  xQueueReset(xQueueMasterResHdl);

  return TRUE;
}

BOOL xMBMasterResEventPost(eMBMasterResEventType eEvent)
{
  BOOL bStatus = TRUE;
  if (bMBPortIsWithinException())
  {
    (void)xQueueSendFromISR(xQueueMasterResHdl, (const void *)&eEvent, pdFALSE);
  }
  else
  {
    (void)xQueueSend(xQueueMasterResHdl, (const void *)&eEvent, pdFALSE);
  }

  return bStatus;
}

BOOL xMBMasterResEventGet(eMBMasterResEventType *eEvent)
{
  BOOL xEventHappened = FALSE;
  if (pdTRUE ==
      xQueueReceive(xQueueMasterResHdl, eEvent, portTICK_RATE_MS * 10))
  {
    xEventHappened = TRUE;
  }
  return xEventHappened;
}

void vMBMasterResErrorCBRespondTimeout()
{
  BOOL ret = xMBMasterResEventPost(EV_RES_ERROR_RESPOND_TIMEOUT);
  if (!ret)
    ESP_LOGE(TAG, "%4d EV_RES_ERROR_RESPOND_TIMEOUT", __LINE__);
}

void vMBMasterResCBRequestScuuess(void)
{
  BOOL ret = xMBMasterResEventPost(EV_RES_PROCESS_SUCESS);
  if (!ret)
    ESP_LOGI(TAG, "%4d EV_RES_PROCESS_SUCESS", __LINE__);
}

void vMBMasterResErrorCBReceiveData()
{
  BOOL ret = xMBMasterResEventPost(EV_RES_ERROR_RECEIVE_DATA);
  if (!ret)
    ESP_LOGE(TAG, "%4d EV_RES_ERROR_RECEIVE_DATA", __LINE__);
}

void vMBMasterResErrorCBExecuteFunction()
{
  BOOL ret = xMBMasterResEventPost(EV_RES_ERROR_EXECUTE_FUNCTION);
  if (!ret)
    ESP_LOGE(TAG, "%4d EV_RES_ERROR_EXECUTE_FUNCTION", __LINE__);
}

eMBMasterReqErrCode eMBMasterWaitRequestFinish(void)
{
  eMBMasterReqErrCode eErrStatus = MB_MRE_NO_ERR;
  eMBMasterResEventType recvedEvent;
RESTART:
  xMBMasterResEventGet(&recvedEvent);
  switch (recvedEvent)
  {
  case EV_RES_PROCESS_SUCESS:
    break;
  case EV_RES_ERROR_RESPOND_TIMEOUT:
  {
    eErrStatus = MB_MRE_TIMEDOUT;
    ESP_LOGE(TAG, "%4d EV_RES_ERROR_RESPOND_TIMEOUT", __LINE__);
    break;
  }
  case EV_RES_ERROR_RECEIVE_DATA:
  {
    eErrStatus = MB_MRE_REV_DATA;
    ESP_LOGE(TAG, "%4d EV_RES_ERROR_RECEIVE_DATA", __LINE__);
    break;
  }
  case EV_RES_ERROR_EXECUTE_FUNCTION:
  {
    eErrStatus = MB_MRE_EXE_FUN;
    ESP_LOGE(TAG, "%4d EV_RES_ERROR_EXECUTE_FUNCTION", __LINE__);
    break;
  }
  default:
    ESP_LOGE(TAG, "%4d unexcept event %d", __LINE__, recvedEvent);
    goto RESTART;
  }
  return eErrStatus;
}

BOOL xMBMasterRunResTake(LONG lTimeOut)
{
  /*If waiting time is -1 .It will wait forever */
  if (xSemaphoreTake(SemaphorMasterHdl, (portTickType)lTimeOut) ==
      pdTRUE) // portMAX_DELAY
  {
    return TRUE;
  }

  return FALSE;
}

void vMBMasterRunResRelease(void)
{
  xSemaphoreGive(SemaphorMasterHdl);
  return;
}

void vMBMasterOsResInit(void) { SemaphorMasterHdl = xSemaphoreCreateMutex(); }
