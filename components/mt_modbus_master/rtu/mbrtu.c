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
 * File: $Id: mbrtu.c,v 1.18 2007/09/12 10:15:56 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "esp_log.h"
#include "esp_timer.h"
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "mt_port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mt_mb.h"
#include "mt_mbframe.h"
#include "mt_mbrtu.h"

#include "mt_mbcrc.h"
#include "mt_mbport.h"
#include "stdio.h"

/* if  MB_DEVICE_USE_TYPE == MB_DEVICE_MASTER  */

/* ----------------------- Defines ------------------------------------------*/
#define MB_SER_PDU_SIZE_MIN 4   /*!< Minimum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_MAX 256 /*!< Maximum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_CRC 2   /*!< Size of CRC field in PDU. */
#define MB_SER_PDU_ADDR_OFF 0   /*!< Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF 1    /*!< Offset of Modbus-PDU in Ser-PDU. */

/* ----------------------- Type definitions ---------------------------------*/
typedef enum
{
  STATE_M_RX_INIT,  /*!< Receiver is in initial state. */
  STATE_M_RX_IDLE,  /*!< Receiver is in idle state. */
  STATE_M_RX_RCV,   /*!< Frame is beeing received. */
  STATE_M_RX_ERROR, /*!< If the frame is invalid. */
} eMBMasterRcvState;

typedef enum
{
  STATE_M_TX_IDLE, /*!< Transmitter is in idle state. */
  STATE_M_TX_XMIT, /*!< Transmitter is in transfer state. */
  STATE_M_TX_XFWR, /*!< Transmitter is in transfer finish and wait receive
                    state. */
} eMBMasterSndState;

/* ----------------------- Static variables ---------------------------------*/
static const char *TAG = "MODBUS_MASTER_MBRTU";
int DEBUG_COUNT = 0;
static volatile eMBMasterSndState eSndState;
static volatile eMBMasterRcvState eRcvState;

static volatile UCHAR ucMasterRTUSndBuf[MB_PDU_SIZE_MAX];
static volatile UCHAR ucMasterRTURcvBuf[MB_SER_PDU_SIZE_MAX];
static volatile USHORT usMasterSendPDULength;

static volatile UCHAR *pucMasterSndBufferCur = NULL;
static volatile USHORT usMasterSndBufferCount;

static volatile USHORT usMasterRcvBufferPos;
static volatile BOOL xFrameIsBroadcast = FALSE;

static volatile eMBMasterTimerMode eMasterCurTimerMode;

/* ----------------------- Start implementation -----------------------------*/
eMBErrorCode eMBMasterRTUInit(UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity,
                              int tx_pin, int rx_pin, int en_pin)
{
  eMBErrorCode eStatus = MB_ENOERR;
  ULONG usTimerT35_50us;

  ENTER_CRITICAL_SECTION();

  /* Modbus RTU uses 8 Databits. */
  if (xMBMasterPortSerialInit(ucPort, ulBaudRate, 8, eParity, tx_pin, rx_pin,
                              en_pin) != TRUE)
  {
    eStatus = MB_EPORTERR;
  }
  else
  {
    /* If baudrate > 19200 then we should use the fixed timer values
     * t35 = 1750us. Otherwise t35 must be 3.5 times the character time.
     */
    if (ulBaudRate > 19200)
    {
      usTimerT35_50us = 35; /* 1800us. */
    }
    else
    {
      /* The timer reload value for a character is given by:
       *
       * ChTimeValue = Ticks_per_1s / ( Baudrate / 11 )
       *             = 11 * Ticks_per_1s / Baudrate
       *             = 220000 / Baudrate
       * The reload for t3.5 is 1.5 times this value and similary
       * for t3.5.
       */
      usTimerT35_50us = (7UL * 220000UL) / (2UL * ulBaudRate);
    }
    if (xMBMasterPortTimersInit((USHORT)usTimerT35_50us) != TRUE)
    {
      eStatus = MB_EPORTERR;
    }
  }
  EXIT_CRITICAL_SECTION();

  return eStatus;
}

void eMBMasterRTUStart(void)
{
  ENTER_CRITICAL_SECTION();
  /* Initially the receiver is in the state STATE_M_RX_INIT. we start
   * the timer and if no character is received within t3.5 we change
   * to STATE_M_RX_IDLE. This makes sure that we delay startup of the
   * modbus protocol stack until the bus is free.
   */
  eRcvState = STATE_M_RX_IDLE;
  vMBMasterPortSerialEnable(TRUE, FALSE);

  EXIT_CRITICAL_SECTION();
}

void eMBMasterRTUStop(void)
{
  ENTER_CRITICAL_SECTION();
  vMBMasterPortSerialEnable(FALSE, FALSE);
  vMBMasterPortTimersDisable();
  EXIT_CRITICAL_SECTION();
}

eMBErrorCode eMBMasterRTUReceive(UCHAR *pucRcvAddress, UCHAR **pucFrame,
                                 USHORT *pusLength)
{
  eMBErrorCode eStatus = MB_ENOERR;

  /*ESP_LOGI(TAG, "RTU RECV");

  for (int i = 0; i < usMasterSendPDULength + 2; i++)
  {
    printf("%2x ", ucMasterRTURcvBuf[i]);
  }
  printf("\n");
  printf("pos:%d, crc:%d\n", usMasterRcvBufferPos,
         usMBCRC16((UCHAR *)ucMasterRTURcvBuf, usMasterRcvBufferPos));
         */

  ENTER_CRITICAL_SECTION();
  assert(usMasterRcvBufferPos < MB_SER_PDU_SIZE_MAX);

  if ((usMasterRcvBufferPos >= MB_SER_PDU_SIZE_MIN) &&
      (usMBCRC16((UCHAR *)ucMasterRTURcvBuf, usMasterRcvBufferPos) == 0))
  {
    /* Save the address field. All frames are passed to the upper layed
     * and the decision if a frame is used is done there.
     */

    *pucRcvAddress = ucMasterRTURcvBuf[MB_SER_PDU_ADDR_OFF];

    /* Total length of Modbus-PDU is Modbus-Serial-Line-PDU minus
     * size of address field and CRC checksum.
     */
    *pusLength = (USHORT)(usMasterRcvBufferPos - MB_SER_PDU_PDU_OFF -
                          MB_SER_PDU_SIZE_CRC);

    /* Return the start of the Modbus PDU to the caller. */
    *pucFrame = (UCHAR *)&ucMasterRTURcvBuf[MB_SER_PDU_PDU_OFF];
  }
  else
  {
    eStatus = MB_EIO;
  }
  EXIT_CRITICAL_SECTION();
  // ESP_LOGI(TAG, "RTU RECV end");
  return eStatus;
}

eMBErrorCode eMBMasterRTUSend(UCHAR ucSlaveAddress, const UCHAR *pucFrame,
                              USHORT usLength)
{
  eMBErrorCode eStatus = MB_ENOERR;
  USHORT usCRC16;

  // printf("eMBMasterRTUSend recv:%d, sent:%d\n", eRcvState, eSndState);

  ENTER_CRITICAL_SECTION();

  /* Check if the receiver is still in idle state. If not we where to
   * slow with processing the received frame and the master sent another
   * frame on the network. We have to abort sending the frame.
   */
  if (eRcvState == STATE_M_RX_IDLE)
  {
    /* First byte before the Modbus-PDU is the slave address. */
    pucMasterSndBufferCur = (UCHAR *)pucFrame - 1;
    usMasterSndBufferCount = 1;

    /* Now copy the Modbus-PDU into the Modbus-Serial-Line-PDU. */
    pucMasterSndBufferCur[MB_SER_PDU_ADDR_OFF] = ucSlaveAddress;
    usMasterSndBufferCount += usLength;

    /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
    usCRC16 = usMBCRC16((UCHAR *)pucMasterSndBufferCur, usMasterSndBufferCount);
    ucMasterRTUSndBuf[usMasterSndBufferCount++] = (UCHAR)(usCRC16 & 0xFF);
    ucMasterRTUSndBuf[usMasterSndBufferCount++] = (UCHAR)(usCRC16 >> 8);

    /* Activate the transmitter. */
    eSndState = STATE_M_TX_XMIT;
    vMBMasterPortSerialEnable(FALSE, TRUE);
  }
  else
  {
    eStatus = MB_EIO;
  }
  EXIT_CRITICAL_SECTION();
  return eStatus;
}

UCHAR get_s_usLength(void) { return usMasterSndBufferCount; }

int set_eRcvState(int st)
{
  eRcvState = st;
  return 0;
}

BOOL xMBMasterRTUReceiveFSM(void)
{
  BOOL xTaskNeedSwitch = FALSE;
  UCHAR ucByte;

  assert((eSndState == STATE_M_TX_IDLE) || (eSndState == STATE_M_TX_XFWR));

  /* Always read the character. */
  (void)xMBMasterPortSerialGetByte((CHAR *)&ucByte);

  //printf("rtu count:%4d recv:%d, sent:%d, pos:%d, byte:%2x time:%lld\n", DEBUG_COUNT, eRcvState, eSndState,
  //       usMasterRcvBufferPos, ucByte, esp_timer_get_time() / 1000);

  switch (eRcvState)
  {
    /* If we have received a character in the init state we have to
       * wait until the frame is finished.
       */
  case STATE_M_RX_INIT:
    vMBMasterPortTimersT35Enable();
    break;

    /* In the error state we wait until all characters in the
       * damaged frame are transmitted.
       */
  case STATE_M_RX_ERROR:
    vMBMasterPortTimersT35Enable();
    break;

    /* In the idle state we wait for a new character. If a character
       * is received the t1.5 and t3.5 timers are started and the
       * receiver is in the state STATE_RX_RECEIVCE and disable early
       * the timer of respond timeout .
       */
  case STATE_M_RX_IDLE:
    /* In time of respond timeout,the receiver receive a frame.
       * Disable timer of respond timeout and change the transmiter state to
       * idle.
       */
    // ESP_LOGI(TAG, "xMBMasterRTUReceiveFSM");
    // ESP_LOGW(TAG, "now state=%d", eRcvState);
    eRcvState = STATE_M_RX_RCV;
    eSndState = STATE_M_TX_IDLE;

    usMasterRcvBufferPos = 0;
    ucMasterRTURcvBuf[usMasterRcvBufferPos++] = ucByte;

    // printf("begin time:%lld\n", esp_timer_get_time() / 1000);

    /* Enable t3.5 timers. */
    vMBMasterPortTimersT35Enable();
    // ESP_LOGI(TAG, "xMBMasterRTUReceiveFSM end");
    break;

    /* We are currently receiving a frame. Reset the timer after
       * every character received. If more than the maximum possible
       * number of bytes in a modbus frame is received the frame is
       * ignored.
       */
  case STATE_M_RX_RCV:
    if (usMasterRcvBufferPos < MB_SER_PDU_SIZE_MAX)
    {
      ucMasterRTURcvBuf[usMasterRcvBufferPos++] = ucByte;
    }
    else
    {
      eRcvState = STATE_M_RX_ERROR;
    }
    vMBMasterPortTimersT35Enable();
    break;
  default:
    ESP_LOGE(TAG, "%4d %s unexcept state:%d", __LINE__, __func__, eRcvState);
  }
  return xTaskNeedSwitch;
}

BOOL xMBMasterRTUTransmitFSM(void)
{
  BOOL xNeedPoll = FALSE;

  assert(eRcvState == STATE_M_RX_IDLE);

  switch (eSndState)
  {
    /* We should not get a transmitter event if the transmitter is in
       * idle state.  */
  case STATE_M_TX_XFWR:

    break;
  case STATE_M_TX_IDLE:
    /* enable receiver/disable transmitter. */
    vMBMasterPortSerialEnable(TRUE, FALSE);
    break;

  case STATE_M_TX_XMIT:
    /* check if we are finished. */
    if (usMasterSndBufferCount != 0)
    {
      xMBMasterPortSerialPutByte((CHAR)*pucMasterSndBufferCur);
      pucMasterSndBufferCur++; /* next byte in sendbuffer. */
      usMasterSndBufferCount--;
    }
    else
    {
      xFrameIsBroadcast =
          (ucMasterRTUSndBuf[MB_SER_PDU_ADDR_OFF] == MB_ADDRESS_BROADCAST)
              ? TRUE
              : FALSE;
      /* Disable transmitter. This prevents another transmit buffer
         * empty interrupt. */
      vMBMasterPortSerialEnable(TRUE, FALSE);
      eSndState = STATE_M_TX_XFWR;
      /* If the frame is broadcast ,master will enable timer of convert delay,
         * else master will enable timer of respond timeout. */
      if (xFrameIsBroadcast == TRUE)
      {
        vMBMasterPortTimersConvertDelayEnable();
      }
      else
      {
        vMBMasterPortTimersRespondTimeoutEnable();
      }
    }
    break;
  }

  return xNeedPoll;
}

BOOL xMBMasterRTUTimerExpired(void)
{
  DEBUG_COUNT++;
  BOOL xNeedPoll = FALSE;
  switch (eRcvState)
  {
    /* Timer t35 expired. Startup phase is finished. */
  case STATE_M_RX_INIT:
    xNeedPoll = xMBMasterPortEventPost(EV_MASTER_READY);
    break;

    /* A frame was received and t35 expired. Notify the listener that
       * a new frame was received. */
  case STATE_M_RX_RCV:
    xNeedPoll = xMBMasterPortEventPost(EV_MASTER_FRAME_RECEIVED);
    break;

    /* An error occured while receiving the frame. */
  case STATE_M_RX_ERROR:
    vMBMasterSetErrorType(EV_RES_ERROR_RECEIVE_DATA);
    xNeedPoll = xMBMasterPortEventPost(EV_MASTER_ERROR_PROCESS);
    break;

    /* Function called in an illegal state. */
  default:
    assert((eRcvState == STATE_M_RX_INIT) || (eRcvState == STATE_M_RX_RCV) ||
           (eRcvState == STATE_M_RX_ERROR) || (eRcvState == STATE_M_RX_IDLE));
    break;
  }
  eRcvState = STATE_M_RX_IDLE;

  switch (eSndState)
  {
    /* A frame was send finish and convert delay or respond timeout expired.
       * If the frame is broadcast,The master will idle,and if the frame is not
       * broadcast.Notify the listener process error.*/
  case STATE_M_TX_XFWR:
    if (xFrameIsBroadcast == FALSE)
    {
      vMBMasterSetErrorType(EV_RES_ERROR_RESPOND_TIMEOUT);
      xNeedPoll = xMBMasterPortEventPost(EV_MASTER_ERROR_PROCESS);
    }
    break;
    /* Function called in an illegal state. */
  default:
    assert((eSndState == STATE_M_TX_XFWR) || (eSndState == STATE_M_TX_IDLE));
    break;
  }
  eSndState = STATE_M_TX_IDLE;

  vMBMasterPortTimersDisable();
  /* If timer mode is convert delay, the master event then turns
   * EV_MASTER_EXECUTE status. */
  if (eMasterCurTimerMode == MB_TMODE_CONVERT_DELAY)
  {
    xNeedPoll = xMBMasterPortEventPost(EV_MASTER_EXECUTE);
  }

  return xNeedPoll;
}

/* Get Modbus Master send RTU's buffer address pointer.*/
void vMBMasterGetRTUSndBuf(UCHAR **pucFrame)
{
  *pucFrame = (UCHAR *)ucMasterRTUSndBuf;
}

/* Get Modbus Master send PDU's buffer address pointer.*/
void vMBMasterGetPDUSndBuf(UCHAR **pucFrame)
{
  *pucFrame = (UCHAR *)&ucMasterRTUSndBuf[MB_SER_PDU_PDU_OFF];
}

/* Set Modbus Master send PDU's buffer length.*/
void vMBMasterSetPDUSndLength(USHORT SendPDULength)
{
  usMasterSendPDULength = SendPDULength;
}

/* Get Modbus Master send PDU's buffer length.*/
USHORT usMBMasterGetPDUSndLength(void) { return usMasterSendPDULength; }

/* Set Modbus Master current timer mode.*/
void vMBMasterSetCurTimerMode(eMBMasterTimerMode eMBTimerMode)
{
  eMasterCurTimerMode = eMBTimerMode;
}

/* The master request is broadcast? */
BOOL xMBMasterRequestIsBroadcast(void) { return xFrameIsBroadcast; }
