#ifndef MODBUS_LD100_H
#define MODBUS_LD100_H

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

eMBErrorCode modbus_init(UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity,
                         int tx_pin, int rx_pin, int en_pin);

eMBMasterReqErrCode modbus_ld100_sync_Cmd_04(UCHAR slaveAddr, USHORT target,
                                             USHORT num, struct RetMsg_t *ret);

eMBMasterReqErrCode modbus_ld100_sync_Cmd_06(UCHAR slaveAddr, USHORT target,
                                             USHORT num, struct RetMsg_t *ret);

esp_err_t mt_ld100_get_addr(int addr, bool *state);

esp_err_t mt_ld100_set_addr(int addr, int new_addr);

esp_err_t mt_ld100_get_state(int addr, bool *state);

esp_err_t mt_ld100_get_relay(int addr, bool *state);

esp_err_t mt_ld100_set_relay(int addr, bool state);

void mt_modbus_task();

#endif