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
 * File: $Id: mbproto.h,v 1.14 2006/12/07 22:10:34 wolti Exp $
 */

#ifndef MT_MB_PROTO_airswitch001_H
#define MT_MB_PROTO_airswitch001_H

#include "mt_mbconfig_airswitch001.h"

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif
/* ----------------------- Defines ------------------------------------------*/
#define MB_ADDRESS_BROADCAST (0) /*! Modbus broadcast address. */
#define MB_ADDRESS_MIN (1)       /*! Smallest possible slave address. */
#define MB_ADDRESS_MAX (247)     /*! Biggest possible slave address. */
#define MB_FUNC_NONE (0)
#define MB_FUNC_01 (1)
#define MB_FUNC_02 (2)
#define MB_FUNC_03 (3)
#define MB_FUNC_04 (4)
#define MB_FUNC_05 (5)
#define MB_FUNC_06 (6)
#define MB_FUNC_ERROR (128)
/* ----------------------- Type definitions ---------------------------------*/
typedef enum
{
  MB_EX_NONE = 0x00,
  MB_EX_ILLEGAL_FUNCTION = 0x01,
  MB_EX_ILLEGAL_DATA_ADDRESS = 0x02,
  MB_EX_ILLEGAL_DATA_VALUE = 0x03,
  MB_EX_SLAVE_DEVICE_FAILURE = 0x04,
  MB_EX_ACKNOWLEDGE = 0x05,
  MB_EX_SLAVE_BUSY = 0x06,
  MB_EX_MEMORY_PARITY_ERROR = 0x08,
  MB_EX_GATEWAY_PATH_FAILED = 0x0A,
  MB_EX_GATEWAY_TGT_FAILED = 0x0B
} eMBException;

typedef eMBException (*pxMBFunctionHandler)(UCHAR *pucFrame, USHORT *pusLength);

typedef struct
{
  UCHAR ucFunctionCode;
  pxMBFunctionHandler pxHandler;
} xMBFunctionHandler;

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
