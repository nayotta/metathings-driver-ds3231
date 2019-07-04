#ifndef MODBUS_LD100_H
#define MODBUS_LD100_H

#include "mt_mb.h"
#include "mt_mbport.h"

#define BUF_MAXLEN 64

struct RetMsg_t  // ret message struct
{
  UCHAR retBuf[BUF_MAXLEN];  // ret buffer
  USHORT recvCmd;            // ret addr
  USHORT retLen;             // ret len
} * RetMsg;                  // global save point

eMBErrorCode modbus_init(UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity,
                         int tx_pin, int rx_pin, int en_pin);

eMBMasterReqErrCode modbus_ld100_sync_Cmd_04(UCHAR slaveAddr, USHORT target,
                                             USHORT num, struct RetMsg_t *ret);

eMBMasterReqErrCode modbus_ld100_sync_Cmd_06(UCHAR slaveAddr, USHORT target,
                                             USHORT num, struct RetMsg_t *ret);

void mt_modbus_task();

#endif