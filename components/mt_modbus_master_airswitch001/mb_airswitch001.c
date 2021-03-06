/*
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mb.c,v 1.28 2010/06/06 13:54:40 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "esp_log.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "mt_port_airswitch001.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mt_mb_airswitch001.h"
#include "mt_mbconfig_airswitch001.h"
#include "mt_mbframe_airswitch001.h"
#include "mt_mbfunc_airswitch001.h"
#include "mt_mbproto_airswitch001.h"

#include "mt_mbport_airswitch001.h"
#include "mt_mbrtu_airswitch001.h"

#ifndef MB_PORT_HAS_CLOSE
#define MB_PORT_HAS_CLOSE 0
#endif

/* ----------------------- Static variables ---------------------------------*/
static UCHAR ucMBMasterDestAddress;
static eMBMasterResEventType eMBMasterCurErrorType;

static enum {
  STATE_ENABLED,
  STATE_DISABLED,
  STATE_NOT_INITIALIZED
} eMBState = STATE_NOT_INITIALIZED;

/* Functions pointer which are initialized in eMBInit( ). Depending on the
 * mode (RTU or ASCII) the are set to the correct implementations.
 * Using for Modbus Master,Add by Armink 20130813
 */
static peMBFrameSend peMBMasterFrameSendCur;
static pvMBFrameStart pvMBMasterFrameStartCur;
static pvMBFrameStop pvMBMasterFrameStopCur;
static peMBFrameReceive peMBMasterFrameReceiveCur;
static pvMBFrameClose pvMBMasterFrameCloseCur;

/* Callback functions required by the porting layer. They are called when
 * an external event has happend which includes a timeout or the reception
 * or transmission of a character.
 * Using for Modbus Master,Add by Armink 20130813
 */
BOOL(*pxMBMasterFrameCBByteReceived)
(void);
BOOL(*pxMBMasterFrameCBTransmitterEmpty)
(void);
BOOL(*pxMBMasterPortCBTimerExpired)
(void);
BOOL(*pxMBMasterFrameCBReceiveFSMCur)
(void);
BOOL(*pxMBMasterFrameCBTransmitFSMCur)
(void);

/* An array of Modbus functions handlers which associates Modbus function
 * codes with implementing functions.
 */
static xMBFunctionHandler xMasterFuncHandlers[MB_FUNC_HANDLERS_MAX] = {
    {MB_FUNC_01, eMBMasterAirswitchFunc01},
    {MB_FUNC_02, eMBMasterAirswitchFunc02},
    {MB_FUNC_03, eMBMasterAirswitchFunc03},
    {MB_FUNC_04, eMBMasterAirswitchFunc04},
    {MB_FUNC_05, eMBMasterAirswitchFunc05},
    {MB_FUNC_06, eMBMasterAirswitchFunc06},
};

/* ----------------------- Start implementation -----------------------------*/
eMBErrorCode eMBMasterAirswitchInit(eMBMode eMode, UCHAR ucPort,
                                    ULONG ulBaudRate, eMBParity eParity,
                                    int tx_pin, int rx_pin, int en_pin) {
  eMBErrorCode eStatus = MB_ENOERR;

  switch (eMode) {
  case MB_RTU:
    pvMBMasterFrameStartCur = eMBMasterAirswitchRTUStart;
    pvMBMasterFrameStopCur = eMBMasterAirswitchRTUStop;
    peMBMasterFrameSendCur = eMBMasterAirswitchRTUSend;
    peMBMasterFrameReceiveCur = eMBMasterAirswitchRTUReceive;
    pvMBMasterFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBMasterAirswitchPortClose : NULL;
    pxMBMasterFrameCBByteReceived = xMBMasterAirswitchRTUReceiveFSM;
    pxMBMasterFrameCBTransmitterEmpty = xMBMasterAirswitchRTUTransmitFSM;
    pxMBMasterPortCBTimerExpired = xMBMasterAirswitchRTUTimerExpired;

    eStatus =
        eMBMasterAirswitchRTUInit(ucPort, ulBaudRate, eParity, tx_pin, rx_pin, en_pin);
    break;
  default:
    eStatus = MB_EINVAL;
    break;
  }

  if (eStatus == MB_ENOERR) {
    // init port event queue
    if (!xMBMasterAirswitchPortEventInit()) {
      /* port dependent event module initalization failed. */
      eStatus = MB_EPORTERR;
    } else {
      eMBState = STATE_DISABLED;
    }

    // init res event queue
    if (!xMBMasterAirswitchResEventInit()) {
      /* port dependent event module initalization failed. */
      eStatus = MB_EPORTERR;
    } else {
      eMBState = STATE_DISABLED;
    }

    /* initialize the OS resource for modbus master. */
    vMBMasterAirswitchOsResInit();
  }
  return eStatus;
}

eMBErrorCode eMBMasterAirswitchClose(void) {
  eMBErrorCode eStatus = MB_ENOERR;

  if (eMBState == STATE_DISABLED) {
    if (pvMBMasterFrameCloseCur != NULL) {
      pvMBMasterFrameCloseCur();
    }
  } else {
    eStatus = MB_EILLSTATE;
  }
  return eStatus;
}

eMBErrorCode eMBMasterAirswitchEnable(void) {
  eMBErrorCode eStatus = MB_ENOERR;

  if (eMBState == STATE_DISABLED) {
    /* Activate the protocol stack. */
    pvMBMasterFrameStartCur();
    eMBState = STATE_ENABLED;
  } else {
    eStatus = MB_EILLSTATE;
  }
  return eStatus;
}

eMBErrorCode eMBMasterAirswitchDisable(void) {
  eMBErrorCode eStatus;

  if (eMBState == STATE_ENABLED) {
    pvMBMasterFrameStopCur();
    eMBState = STATE_DISABLED;
    eStatus = MB_ENOERR;
  } else if (eMBState == STATE_DISABLED) {
    eStatus = MB_ENOERR;
  } else {
    eStatus = MB_EILLSTATE;
  }
  return eStatus;
}

eMBErrorCode eMBMasterAirswitchPoll(void) {
  static UCHAR *ucMBFrame;
  static UCHAR ucRcvAddress;
  static UCHAR ucFunctionCode;
  static USHORT usLength;
  static eMBException eException;

  int i, j;
  eMBErrorCode eStatus = MB_ENOERR;
  eMBMasterEventType eEvent;
  eMBMasterResEventType errorType;

  /* Check if the protocol stack is ready. */
  if (eMBState != STATE_ENABLED) {
    return MB_EILLSTATE;
  }

  /* Check if there is a event available. If not return control to caller.
   * Otherwise we will handle the event. */
  if (xMBMasterAirswitchPortEventGet(&eEvent) == TRUE) {
    switch (eEvent) {
    case EV_MASTER_PROCESS_SUCESS: // 0x20
      break;
    case EV_MASTER_ERROR_RESPOND_TIMEOUT: // 0x40
      break;
    case EV_MASTER_ERROR_RECEIVE_DATA: // 0x80
      break;
    case EV_MASTER_ERROR_EXECUTE_FUNCTION: // 0x100
      break;
    case EV_MASTER_READY: // 0x01
      break;
    case EV_MASTER_FRAME_RECEIVED: // 0x02
      eStatus = peMBMasterFrameReceiveCur(&ucRcvAddress, &ucMBFrame, &usLength);
      /* Check if the frame is for us. If not ,send an error process event. */
      if ((eStatus == MB_ENOERR) &&
          (ucRcvAddress == ucMBMasterAirswitchGetDestAddress())) {
        xMBMasterAirswitchPortEventPost(EV_MASTER_EXECUTE);
      } else {
        // printf("debug:%d rcvadd=%d getadd=%d\n",
        // eStatus,ucRcvAddress,ucMBMasterAirswitchGetDestAddress());
        vMBMasterAirswitchSetErrorType(EV_RES_ERROR_RECEIVE_DATA);
        xMBMasterAirswitchPortEventPost(EV_MASTER_ERROR_PROCESS);
      }
      break;

    case EV_MASTER_EXECUTE: // 0x04
      ucFunctionCode = ucMBFrame[MB_PDU_FUNC_OFF];
      eException = MB_EX_ILLEGAL_FUNCTION;
      /* If receive frame has exception .The receive function code highest bit
       * is 1.*/
      if (ucFunctionCode >> 7) {
        eException = (eMBException)ucMBFrame[MB_PDU_DATA_OFF];
      } else {
        for (i = 0; i < MB_FUNC_HANDLERS_MAX; i++) {
          /* No more function handlers registered. Abort. */
          if (xMasterFuncHandlers[i].ucFunctionCode == 0) {
            break;
          } else if (xMasterFuncHandlers[i].ucFunctionCode == ucFunctionCode) {
            /* If master request is broadcast,
             * the master need execute function for all slave.
             */
            if (xMBMasterAirswitchRequestIsBroadcast()) {
              usLength = usMBMasterAirswitchGetPDUSndLength();
              for (j = 1; j <= MB_MASTER_TOTAL_SLAVE_NUM; j++) {
                vMBMasterAirswitchSetDestAddress(j);
                eException =
                    xMasterFuncHandlers[i].pxHandler(ucMBFrame, &usLength);
              }
            } else {
              eException =
                  xMasterFuncHandlers[i].pxHandler(ucMBFrame, &usLength);
            }
            break;
          }
        }
      }
      /* If master has exception ,Master will send error process.Otherwise the
       * Master is idle.*/
      if (eException != MB_EX_NONE) {
        vMBMasterAirswitchSetErrorType(EV_RES_ERROR_EXECUTE_FUNCTION);
        (void)xMBMasterAirswitchPortEventPost(EV_MASTER_ERROR_PROCESS);
      } else {
        vMBMasterAirswitchResCBRequestScuuess();
        vMBMasterAirswitchRunResRelease();
      }
      break;

    case EV_MASTER_FRAME_SENT: // 0x08
      /* Master is busy now. */
      vMBMasterAirswitchGetPDUSndBuf(&ucMBFrame);
      eStatus = peMBMasterFrameSendCur(ucMBMasterAirswitchGetDestAddress(), ucMBFrame,
                                       usMBMasterAirswitchGetPDUSndLength());
      break;

    case EV_MASTER_ERROR_PROCESS: // 0x10
      /* Execute specified error process callback function. */
      errorType = eMBMasterAirswitchGetErrorType();
      vMBMasterAirswitchGetPDUSndBuf(&ucMBFrame);
      switch (errorType) {
      case EV_RES_ERROR_RESPOND_TIMEOUT:
        vMBMasterAirswitchResErrorCBRespondTimeout(ucMBFrame,
                                          usMBMasterAirswitchGetPDUSndLength());
        break;
      case EV_RES_ERROR_RECEIVE_DATA:
        vMBMasterAirswitchResErrorCBReceiveData(ucMBFrame, usMBMasterAirswitchGetPDUSndLength());
        break;
      case EV_RES_ERROR_EXECUTE_FUNCTION:
        vMBMasterAirswitchResErrorCBExecuteFunction(ucMBFrame,
                                           usMBMasterAirswitchGetPDUSndLength());
        break;
      case EV_RES_PROCESS_SUCESS:
        break;
      }
      vMBMasterAirswitchRunResRelease();
      break;
    default:
      vMBMasterAirswitchRunResRelease();
      break;
    }
  }
  return MB_ENOERR;
}

/* Get Modbus Master send destination address. */
UCHAR ucMBMasterAirswitchGetDestAddress(void) { return ucMBMasterDestAddress; }
/* Set Modbus Master send destination address. */
void vMBMasterAirswitchSetDestAddress(UCHAR Address) {
  ucMBMasterDestAddress = Address;
}

/* Get Modbus Master current error event type. */
eMBMasterResEventType eMBMasterAirswitchGetErrorType(void) {
  return eMBMasterCurErrorType;
}
/* Set Modbus Master current error event type. */
void vMBMasterAirswitchSetErrorType(eMBMasterResEventType errorType) {
  eMBMasterCurErrorType = errorType;
}
