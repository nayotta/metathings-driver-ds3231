#ifndef _MT_MBTASK_H
#define _MT_MBTASK_H

#include "mt_mb.h"
#include "mt_mbport.h"
#include "esp_err.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

#define BUF_MAXLEN 64

struct RetMsg_t // ret message struct
{
    UCHAR retBuf[BUF_MAXLEN]; // ret buffer
    USHORT recvCmd;           // ret addr
    USHORT retLen;            // ret len
} * RetMsg;                   // global save point

eMBErrorCode modbus_init(UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity,
                         int tx_pin, int rx_pin, int en_pin);

eMBMasterReqErrCode modbus_sync_Cmd_01(UCHAR slaveAddr, USHORT target,
                                       USHORT num, struct RetMsg_t *ret);

eMBMasterReqErrCode modbus_sync_Cmd_02(UCHAR slaveAddr, USHORT target,
                                       USHORT num, struct RetMsg_t *ret);

eMBMasterReqErrCode modbus_sync_Cmd_03(UCHAR slaveAddr, USHORT target,
                                       USHORT num, struct RetMsg_t *ret);

eMBMasterReqErrCode modbus_sync_Cmd_04(UCHAR slaveAddr, USHORT target,
                                       USHORT num, struct RetMsg_t *ret);

eMBMasterReqErrCode modbus_sync_Cmd_05(UCHAR slaveAddr, USHORT target,
                                       USHORT num, struct RetMsg_t *ret);

eMBMasterReqErrCode modbus_sync_Cmd_06(UCHAR slaveAddr, USHORT target,
                                       USHORT num, struct RetMsg_t *ret);

void mt_modbus_task();

#endif