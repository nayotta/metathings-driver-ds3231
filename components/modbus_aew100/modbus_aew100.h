#ifndef _MODBUS_AEW100_H
#define _MODBUS_AEW100_H

#include "mt_mb.h"
#include "mt_mbport.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

#define BUF_MAXLEN 64

struct RetMsg_t  // ret message struct
{
  UCHAR retBuf[BUF_MAXLEN];  // ret buffer
  USHORT recvCmd;            // ret addr
  USHORT retLen;             // ret len
} * RetMsg;                  // global save point

eMBErrorCode modbus_aew100_init(UCHAR ucPort, ULONG ulBaudRate,
                                eMBParity eParity, int tx_pin, int rx_pin,
                                int en_pin);

eMBMasterReqErrCode modbus_aew100_sync_cmd_03(UCHAR slaveAddr, USHORT target,
                                              USHORT num, struct RetMsg_t *ret);

esp_err_t mt_aew100_get_current(UCHAR addr, double *currentA, double *currentB,
                                double *currentC);

void mt_modbus_aew100_task();

#endif