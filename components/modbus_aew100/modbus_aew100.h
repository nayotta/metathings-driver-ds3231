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

struct _Aew100_data_t
{
  double currentA;
  double currentB;
  double currentC;
  double votageA;
  double votageB;
  double votageC;
  double activePowerA;
  double activePowerB;
  double activePowerC;
  double reactivePowerA;
  double reactivePowerB;
  double reactivePowerC;
  double powerFactorA;
  double powerFactorB;
  double powerFactorC;
  double qualityA;
  double qualityB;
  double qualityC;
  double tempA;
  double tempB;
  double tempC;
  double maxPowerA;
  double maxPowerB;
  double maxPowerC;
};

typedef struct _Aew100_data_t Aew100_data_t;

struct RetMsg_t // ret message struct
{
  UCHAR retBuf[BUF_MAXLEN]; // ret buffer
  USHORT recvCmd;           // ret addr
  USHORT retLen;            // ret len
} * RetMsg;                 // global save point

eMBErrorCode modbus_aew100_init(UCHAR ucPort, ULONG ulBaudRate,
                                eMBParity eParity, int tx_pin, int rx_pin,
                                int en_pin);

eMBMasterReqErrCode modbus_aew100_sync_cmd_03(UCHAR slaveAddr, USHORT target,
                                              USHORT num, struct RetMsg_t *ret);

esp_err_t mt_aew100_get_currentA(UCHAR addr, double *data);
esp_err_t mt_aew100_get_currentB(UCHAR addr, double *data);
esp_err_t mt_aew100_get_currentC(UCHAR addr, double *data);

esp_err_t mt_aew100_get_votageA(UCHAR addr, double *data);
esp_err_t mt_aew100_get_votageB(UCHAR addr, double *data);
esp_err_t mt_aew100_get_votageC(UCHAR addr, double *data);

esp_err_t mt_aew100_get_activePowerA(UCHAR addr, double *data);
esp_err_t mt_aew100_get_activePowerB(UCHAR addr, double *data);
esp_err_t mt_aew100_get_activePowerC(UCHAR addr, double *data);

esp_err_t mt_aew100_get_reactivePowerA(UCHAR addr, double *data);
esp_err_t mt_aew100_get_reactivePowerB(UCHAR addr, double *data);
esp_err_t mt_aew100_get_reactivePowerC(UCHAR addr, double *data);

esp_err_t mt_aew100_get_powerFactorA(UCHAR addr, double *data);
esp_err_t mt_aew100_get_powerFactorB(UCHAR addr, double *data);
esp_err_t mt_aew100_get_powerFactorC(UCHAR addr, double *data);

esp_err_t mt_aew100_get_qualityA(UCHAR addr, double *data);
esp_err_t mt_aew100_get_qualityB(UCHAR addr, double *data);
esp_err_t mt_aew100_get_qualityC(UCHAR addr, double *data);

esp_err_t mt_aew100_get_tempA(UCHAR addr, double *data);
esp_err_t mt_aew100_get_tempB(UCHAR addr, double *data);
esp_err_t mt_aew100_get_tempC(UCHAR addr, double *data);

esp_err_t mt_aew100_get_maxPowerA(UCHAR addr, double *data);
esp_err_t mt_aew100_get_maxPowerB(UCHAR addr, double *data);
esp_err_t mt_aew100_get_maxPowerC(UCHAR addr, double *data);

esp_err_t mt_aew100_get_data(UCHAR addr, Aew100_data_t *data);

void mt_modbus_aew100_task();

#endif