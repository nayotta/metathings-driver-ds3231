#ifndef MODBUS_xycwa6a_H
#define MODBUS_xycwa6a_H

#include "mt_mb.h"
#include "mt_mbport.h"

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
} * RetMsg;                 // global save point

eMBErrorCode modbus_xycwa6a_init(UCHAR ucPort, ULONG ulBaudRate,
                                 eMBParity eParity, int tx_pin, int rx_pin,
                                 int en_pin);

esp_err_t mt_xycwa6a_get_temps(int addr, double *temp1,
                               double *temp2, double *temp3, double *temp4,
                               double *temp5, double *temp6);

void mt_modbus_xycwa6a_task();

#endif