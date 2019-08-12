
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
 * File: $Id: mbfuncnyt.c,v 1.0 2019/01/04 18:47:16 nayotta zh $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "esp_log.h"

/* ----------------------- Platform includes --------------------------------*/
#include "mt_port_airswitch001.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mt_mb_airswitch001.h"
#include "mt_mbconfig_airswitch001.h"
#include "mt_mbframe_airswitch001.h"
#include "mt_mbproto_airswitch001.h"

/* ----------------------- Defines ------------------------------------------*/
static const char *TAG = "MT_MODBUS_MASTER_airswitch001";
// send parse
#define MB_PDU_REQ_TARGET_OFF (MB_PDU_DATA_OFF + 0)
#define MB_PDU_REQ_SUBCMD_OFF (MB_PDU_DATA_OFF + 0)
#define MB_PDU_REQ_SUBTARGET_OFF (MB_PDU_DATA_OFF + 1)
#define MB_PDU_REQ_NUM_OFF (MB_PDU_DATA_OFF + 2)
#define MB_PDU_REQ_CMD_SIZE (4)
// recv parse
#define MB_PDU_FUNC_01_NUM_OFF (MB_PDU_DATA_OFF + 0)
#define MB_PDU_FUNC_02_NUM_OFF (MB_PDU_DATA_OFF + 0)
#define MB_PDU_FUNC_03_NUM_OFF (MB_PDU_DATA_OFF + 0)
#define MB_PDU_FUNC_04_NUM_OFF (MB_PDU_DATA_OFF + 0)
#define MB_PDU_FUNC_05_NUM_OFF (MB_PDU_DATA_OFF + 0) // no need
#define MB_PDU_FUNC_06_NUM_OFF (MB_PDU_DATA_OFF + 0) // no need
#define MB_PDU_FUNC_01_VALUES_OFF (MB_PDU_DATA_OFF + 1)
#define MB_PDU_FUNC_02_VALUES_OFF (MB_PDU_DATA_OFF + 1)
#define MB_PDU_FUNC_03_VALUES_OFF (MB_PDU_DATA_OFF + 1)
#define MB_PDU_FUNC_04_VALUES_OFF (MB_PDU_DATA_OFF + 1)
#define MB_PDU_FUNC_05_VALUES_OFF (MB_PDU_DATA_OFF + 2)
#define MB_PDU_FUNC_06_VALUES_OFF (MB_PDU_DATA_OFF + 2)

/* ----------------------- Start implementation -----------------------------*/
// cmd 01 读取开关状态
/*
    01 slaveAddr
    02 cmd 0x01
    03 addr high
    04 addr low
    05 num high
    06 num low
*/

eMBMasterReqErrCode eMBMasterAirswitchReq01(UCHAR slaveAddr, USHORT target, USHORT num,
                                   LONG lTimeOut)
{
  UCHAR *frame = NULL;
  eMBMasterReqErrCode err = MB_MRE_NO_ERR;

  if (xMBMasterRunResTake(lTimeOut) == FALSE)
    err = MB_MRE_MASTER_BUSY;
  else
  {
    vMBMasterGetPDUSndBuf(&frame);
    vMBMasterSetDestAddress(slaveAddr);
    frame[MB_PDU_FUNC_OFF] = 1;
    frame[MB_PDU_REQ_TARGET_OFF] = target >> 8;
    frame[MB_PDU_REQ_TARGET_OFF + 1] = target;
    frame[MB_PDU_REQ_NUM_OFF] = num >> 8;
    frame[MB_PDU_REQ_NUM_OFF + 1] = num;
    vMBMasterSetPDUSndLength(MB_PDU_SIZE_MIN + MB_PDU_REQ_CMD_SIZE);
    (void)xMBMasterResEventClean();
    (void)xMBMasterPortEventPost(EV_MASTER_FRAME_SENT);
    err = eMBMasterWaitRequestFinish();
  }
  return err;
}

eMBException eMBMasterAirswitchFunc01(UCHAR *recvFrame, USHORT *recvBit)
{
  UCHAR *sendFrame = NULL;
  UCHAR recvCmd;
  USHORT recvNum;
  UCHAR recvLen;

  eMBException eStatus = MB_EX_NONE;
  eMBErrorCode eRetStatus;

  if (xMBMasterRequestIsBroadcast())
  {
    eStatus = MB_EX_NONE;
  }
  else if (*recvBit >= 2)
  {
    vMBMasterGetPDUSndBuf(&sendFrame);
    recvCmd = (USHORT)(sendFrame[MB_PDU_FUNC_OFF]);

    recvNum = (USHORT)(sendFrame[MB_PDU_REQ_NUM_OFF] << 8);
    recvNum |= (USHORT)(sendFrame[MB_PDU_REQ_NUM_OFF + 1]);

    if ((recvNum & 0x0007) != 0)
    {
      recvLen = (UCHAR)(recvNum / 8 + 1);
    }
    else
    {
      recvLen = (UCHAR)(recvNum / 8);
    }

    // special command
    if (recvNum == 0xFF)
    {
      recvLen = recvFrame[MB_PDU_FUNC_01_NUM_OFF];
    }

    if ((recvNum >= 1) && recvFrame[MB_PDU_FUNC_01_NUM_OFF] == recvLen)
    {
      eRetStatus = eMBMasterCB01(&recvFrame[MB_PDU_FUNC_01_VALUES_OFF], recvCmd,
                                 recvLen);
      if (eRetStatus != MB_ENOERR)
      {
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
      }
    }
    else
    {
      eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
  }
  else
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}

// cmd 02 读取开关是否能被控制
/*
    01 slaveAddr
    02 cmd 0x02
    03 switch high
    04 switch low
    05 num high
    06 num low
*/
eMBMasterReqErrCode eMBMasterAirswitchReq02(UCHAR slaveAddr, USHORT target, USHORT num,
                                   LONG lTimeOut)
{
  UCHAR *frame = NULL;
  eMBMasterReqErrCode err = MB_MRE_NO_ERR;

  if (xMBMasterRunResTake(lTimeOut) == FALSE)
    err = MB_MRE_MASTER_BUSY;
  else
  {
    vMBMasterGetPDUSndBuf(&frame);
    vMBMasterSetDestAddress(slaveAddr);
    frame[MB_PDU_FUNC_OFF] = 2;
    frame[MB_PDU_REQ_TARGET_OFF] = target >> 8;
    frame[MB_PDU_REQ_TARGET_OFF + 1] = target;
    frame[MB_PDU_REQ_NUM_OFF] = num >> 8;
    frame[MB_PDU_REQ_NUM_OFF + 1] = num;
    vMBMasterSetPDUSndLength(MB_PDU_SIZE_MIN + MB_PDU_REQ_CMD_SIZE);
    (void)xMBMasterResEventClean();
    (void)xMBMasterPortEventPost(EV_MASTER_FRAME_SENT);
    err = eMBMasterWaitRequestFinish();
  }
  return err;
}

eMBException eMBMasterAirswitchFunc02(UCHAR *recvFrame, USHORT *recvBit)
{
  UCHAR *sendFrame = NULL;
  UCHAR recvCmd;
  USHORT recvNum;
  UCHAR recvLen;

  eMBException eStatus = MB_EX_NONE;
  eMBErrorCode eRetStatus;

  if (xMBMasterRequestIsBroadcast())
  {
    eStatus = MB_EX_NONE;
  }
  else if (*recvBit >= 2)
  {
    vMBMasterGetPDUSndBuf(&sendFrame);
    recvCmd = (USHORT)(sendFrame[MB_PDU_FUNC_OFF]);

    recvNum = (USHORT)(sendFrame[MB_PDU_REQ_NUM_OFF] << 8);
    recvNum |= (USHORT)(sendFrame[MB_PDU_REQ_NUM_OFF + 1]);

    if ((recvNum & 0x0007) != 0)
    {
      recvLen = (UCHAR)(recvNum / 8 + 1);
    }
    else
    {
      recvLen = (UCHAR)(recvNum / 8);
    }

    // special command
    if (recvNum == 0xFF)
    {
      recvLen = recvFrame[MB_PDU_FUNC_01_NUM_OFF];
    }

    if ((recvNum >= 1) && recvFrame[MB_PDU_FUNC_02_NUM_OFF] == recvLen)
    {
      eRetStatus = eMBMasterCB02(&recvFrame[MB_PDU_FUNC_02_VALUES_OFF], recvCmd,
                                 recvLen);
      if (eRetStatus != MB_ENOERR)
      {
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
      }
    }
    else
    {
      eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
  }
  else
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}

// cmd 03 读从机实时状态
/*
    01 slaveAddr
    02 cmd 0x03
    03 sub cmd 0x01-0x15
    04 switch num
    05 num high
    06 num low
*/
eMBMasterReqErrCode eMBMasterAirswitchReq03(UCHAR slaveAddr, UCHAR subCmd, UCHAR target,
                                   USHORT num, LONG lTimeOut)
{
  UCHAR *frame = NULL;
  eMBMasterReqErrCode err = MB_MRE_NO_ERR;

  if (xMBMasterRunResTake(lTimeOut) == FALSE)
    err = MB_MRE_MASTER_BUSY;
  else
  {
    vMBMasterGetPDUSndBuf(&frame);
    vMBMasterSetDestAddress(slaveAddr);
    frame[MB_PDU_FUNC_OFF] = 3;
    frame[MB_PDU_REQ_TARGET_OFF] = subCmd;
    frame[MB_PDU_REQ_TARGET_OFF + 1] = target;
    frame[MB_PDU_REQ_NUM_OFF] = num >> 8;
    frame[MB_PDU_REQ_NUM_OFF + 1] = num;
    vMBMasterSetPDUSndLength(MB_PDU_SIZE_MIN + MB_PDU_REQ_CMD_SIZE);
    xMBMasterResEventClean();
    xMBMasterPortEventPost(EV_MASTER_FRAME_SENT);
    err = eMBMasterWaitRequestFinish();
  }
  return err;
}

eMBException eMBMasterAirswitchFunc03(UCHAR *recvFrame, USHORT *recvBit)
{
  UCHAR *sendFrame = NULL;
  UCHAR recvCmd;
  USHORT recvNum;

  eMBException eStatus = MB_EX_NONE;
  eMBErrorCode eRetStatus;

  if (xMBMasterRequestIsBroadcast())
  {
    eStatus = MB_EX_NONE;
  }
  else if (*recvBit >= 2)
  {
    vMBMasterGetPDUSndBuf(&sendFrame);
    recvCmd = (USHORT)(sendFrame[MB_PDU_FUNC_OFF]);

    recvNum = (USHORT)(sendFrame[MB_PDU_REQ_NUM_OFF] << 8);
    recvNum |= (USHORT)(sendFrame[MB_PDU_REQ_NUM_OFF + 1]);

    if ((recvNum >= 1) && recvFrame[MB_PDU_FUNC_03_NUM_OFF] == recvNum * 2)
    {
      eRetStatus = eMBMasterCB03(&recvFrame[MB_PDU_FUNC_03_VALUES_OFF], recvCmd,
                                 recvNum * 2);
      if (eRetStatus != MB_ENOERR)
      {
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
      }
    }
    else
    {
      eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
  }
  else
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}

// cmd 04 读从机参数
/*
    01 slaveAddr
    02 cmd 0x04
    03 sub cmd 0x01-0x07
    04 switch num
    05 num high
    06 num low
*/
eMBMasterReqErrCode eMBMasterAirswitchReq04(UCHAR slaveAddr, UCHAR subCmd, UCHAR target,
                                   USHORT num, LONG lTimeOut)
{
  UCHAR *frame = NULL;
  eMBMasterReqErrCode err = MB_MRE_NO_ERR;

  if (xMBMasterRunResTake(lTimeOut) == FALSE)
  {
    err = MB_MRE_MASTER_BUSY;
  }
  else
  {
    vMBMasterGetPDUSndBuf(&frame);
    vMBMasterSetDestAddress(slaveAddr);
    frame[MB_PDU_FUNC_OFF] = 4;
    frame[MB_PDU_REQ_TARGET_OFF] = subCmd;
    frame[MB_PDU_REQ_TARGET_OFF + 1] = target;
    frame[MB_PDU_REQ_NUM_OFF] = num >> 8;
    frame[MB_PDU_REQ_NUM_OFF + 1] = num;
    vMBMasterSetPDUSndLength(MB_PDU_SIZE_MIN + MB_PDU_REQ_CMD_SIZE);
    (void)xMBMasterResEventClean();
    (void)xMBMasterPortEventPost(EV_MASTER_FRAME_SENT);
    err = eMBMasterWaitRequestFinish();
  }
  return err;
}

eMBException eMBMasterAirswitchFunc04(UCHAR *recvFrame, USHORT *recvBit)
{
  UCHAR *sendFrame = NULL;
  UCHAR recvCmd;
  USHORT recvNum;

  eMBException eStatus = MB_EX_NONE;
  eMBErrorCode eRetStatus;

  if (xMBMasterRequestIsBroadcast())
  {
    eStatus = MB_EX_NONE;
  }
  else if (*recvBit >= 2)
  {
    vMBMasterGetPDUSndBuf(&sendFrame);
    recvCmd = (USHORT)(sendFrame[MB_PDU_FUNC_OFF]);

    recvNum = (USHORT)(sendFrame[MB_PDU_REQ_NUM_OFF] << 8);
    recvNum |= (USHORT)(sendFrame[MB_PDU_REQ_NUM_OFF + 1]);

    if ((recvNum >= 1) && recvFrame[MB_PDU_FUNC_04_NUM_OFF] == recvNum * 2)
    {
      eRetStatus = eMBMasterCB04(&recvFrame[MB_PDU_FUNC_04_VALUES_OFF], recvCmd,
                                 recvNum * 2);
      if (eRetStatus != MB_ENOERR)
      {
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
      }
    }
    else
    {
      eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
  }
  else
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}

// cmd 05 控制开关
/*
    01 slaveAddr
    02 cmd 0x05
    03 addr high
    04 addr low
    05 num high
    06 num low
*/
eMBMasterReqErrCode eMBMasterAirswitchReq05(UCHAR slaveAddr, USHORT target, USHORT num,
                                   LONG lTimeOut)
{
  UCHAR *frame = NULL;
  eMBMasterReqErrCode err = MB_MRE_NO_ERR;

  if (xMBMasterRunResTake(lTimeOut) == FALSE)
    err = MB_MRE_MASTER_BUSY;
  else
  {
    vMBMasterGetPDUSndBuf(&frame);
    vMBMasterSetDestAddress(slaveAddr);
    frame[MB_PDU_FUNC_OFF] = 5;
    frame[MB_PDU_REQ_TARGET_OFF] = target >> 8;
    frame[MB_PDU_REQ_TARGET_OFF + 1] = target;
    frame[MB_PDU_REQ_NUM_OFF] = num >> 8;
    frame[MB_PDU_REQ_NUM_OFF + 1] = num;
    vMBMasterSetPDUSndLength(MB_PDU_SIZE_MIN + MB_PDU_REQ_CMD_SIZE);
    (void)xMBMasterResEventClean();
    (void)xMBMasterPortEventPost(EV_MASTER_FRAME_SENT);
    err = eMBMasterWaitRequestFinish();
  }
  return err;
}

eMBException eMBMasterAirswitchFunc05(UCHAR *recvFrame, USHORT *recvBit)
{
  UCHAR *sendFrame = NULL;
  UCHAR recvCmd;
  USHORT recvNum;

  eMBException eStatus = MB_EX_NONE;
  eMBErrorCode eRetStatus;

  if (xMBMasterRequestIsBroadcast())
  {
    eStatus = MB_EX_NONE;
  }
  else if (*recvBit >= 2)
  {
    vMBMasterGetPDUSndBuf(&sendFrame);
    recvCmd = (USHORT)(sendFrame[MB_PDU_FUNC_OFF]);

    recvNum = 2;

    eRetStatus =
        eMBMasterCB05(&recvFrame[MB_PDU_FUNC_05_VALUES_OFF], recvCmd, recvNum);
    if (eRetStatus != MB_ENOERR)
    {
      eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
  }
  else
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}

// cmd 06 写单个从机参数
/*
    01 slaveAddr
    02 cmd 0x06
    03 sub cmd 0x01-0x17
    04 switch num
    05 num high
    06 num low
*/
eMBMasterReqErrCode eMBMasterAirswitchReq06(UCHAR slaveAddr, UCHAR subCmd, UCHAR target,
                                   USHORT num, LONG lTimeOut)
{
  UCHAR *frame = NULL;
  eMBMasterReqErrCode err = MB_MRE_NO_ERR;

  if (xMBMasterRunResTake(lTimeOut) == FALSE)
    err = MB_MRE_MASTER_BUSY;
  else
  {
    vMBMasterGetPDUSndBuf(&frame);
    vMBMasterSetDestAddress(slaveAddr);
    frame[MB_PDU_FUNC_OFF] = 6;
    frame[MB_PDU_REQ_TARGET_OFF] = subCmd;
    frame[MB_PDU_REQ_TARGET_OFF + 1] = target;
    frame[MB_PDU_REQ_NUM_OFF] = num >> 8;
    frame[MB_PDU_REQ_NUM_OFF + 1] = num;
    vMBMasterSetPDUSndLength(MB_PDU_SIZE_MIN + MB_PDU_REQ_CMD_SIZE);
    (void)xMBMasterResEventClean();
    (void)xMBMasterPortEventPost(EV_MASTER_FRAME_SENT);
    err = eMBMasterWaitRequestFinish();
  }
  return err;
}

eMBException eMBMasterAirswitchFunc06(UCHAR *recvFrame, USHORT *recvBit)
{
  UCHAR *sendFrame = NULL;
  UCHAR recvCmd;
  USHORT recvNum;

  eMBException eStatus = MB_EX_NONE;
  eMBErrorCode eRetStatus;

  if (xMBMasterRequestIsBroadcast())
  {
    eStatus = MB_EX_NONE;
  }
  else if (*recvBit >= 2)
  {
    vMBMasterGetPDUSndBuf(&sendFrame);
    recvCmd = (USHORT)(sendFrame[MB_PDU_FUNC_OFF]);

    recvNum = 2;

    eRetStatus =
        eMBMasterCB06(&recvFrame[MB_PDU_FUNC_06_VALUES_OFF], recvCmd, recvNum);
    if (eRetStatus != MB_ENOERR)
    {
      eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
  }
  else
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}

// cmd 15 批量写开关

// cmd 16 批量写从机参数