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
 * File: $Id: mbrtu.h,v 1.9 2006/12/07 22:10:34 wolti Exp $
 */

#ifndef MT_MB_RTU_H
#define MT_MB_RTU_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

UCHAR get_s_usLength(void);
int set_eRcvState(int st);

/* if  MB_DEVICE_USE_TYPE == MB_DEVICE_MASTER  */

eMBErrorCode eMBMasterRTUInit(UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity,
                              int tx_pin, int rx_pin, int en_pin);
void eMBMasterRTUStart(void);
void eMBMasterRTUStop(void);
eMBErrorCode eMBMasterRTUReceive(UCHAR *pucRcvAddress, UCHAR **pucFrame,
                                 USHORT *pusLength);
eMBErrorCode eMBMasterRTUSend(UCHAR slaveAddress, const UCHAR *pucFrame,
                              USHORT usLength);
BOOL xMBMasterRTUReceiveFSM(void);
BOOL xMBMasterRTUTransmitFSM(void);
BOOL xMBMasterRTUTimerExpired(void);

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
