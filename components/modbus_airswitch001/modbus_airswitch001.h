#ifndef MODBUS_airswitch001_H
#define MODBUS_airswitch001_H

#include "esp_err.h"

#include "mt_mb_airswitch001.h"
#include "mt_mbfunc_airswitch001.h"
#include "mt_mbport_airswitch001.h"
#include "mt_port_airswitch001.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

#define BUF_MAXLEN 64

typedef enum
{
  NO_LOG = 0,
  WITH_LOG = 1,
} Airswitch_Log_Type;

typedef enum
{
  ADDRS_READ = 0x01,
} Airswitch_Addrs_Read_Type; // cmd 01

typedef enum
{
  CTRL_READ = 0x01,
} Airswitch_Ctrl_Read_Type; // cmd 02

typedef enum
{
  DATA_VOTAGE = 0x00,
  DATA_LEAK_CURRENT = 0x01,
  DATA_POWER = 0x02,
  DATA_TEMP = 0x03,
  DATA_CURRENT = 0x04,
  DATA_WARN = 0x05,
  DATA_QUALITY_L = 0x06,
  DATA_QUALITY_H = 0x07,
} Airswitch_Data_Read_Type; // cmd 03

typedef enum
{
  CFG_READ_VOTAGE_HIGH = 0x00,
  CFG_READ_VOTAGE_LOW = 0x01,
  CFG_READ_LEAK_CURRENT = 0x02,
  CFG_READ_POWER_HIGH = 0x03,
  CFG_READ_TEMP_HIGH = 0x04,
  CFG_READ_CURRENT_HIGH = 0x05,
  CFG_READ_MODEL = 0x06,
} Airswitch_Cfg_Read_Type; // cmd 04

typedef enum
{
  SET_STATE = 0x05,
} Airswitch_Set_State_Type; // cmd 05

typedef enum
{
  CFG_WRITE_VOTAGE_HIGH = 0x00,
  CFG_WRITE_VOTAGE_LOW = 0x01,
  CFG_WRITE_LEAK_CURRENT_HIGH = 0x02,
  CFG_WRITE_POWER_HIGH = 0x03,
  CFG_WRITE_TEMP_HIGH = 0x04,
  CFG_WRITE_CURRENT_HIGH = 0x05,
  CFG_WRITE_SET_VOTAGE = 0x06,
  CFG_WRITE_SET_STAT = 0x07,
  CFG_WRITE_LEAK_TEST = 0x08,
  CFG_WRITE_WARN_VOTAGE_HIGH = 0x14,
  CFG_WRITE_WARN_VOTAGE_LOW = 0x15,
  CFG_WRITE_QUALITY_H = 0x16,
  CFG_WRITE_QUALITY_L = 0x17,
} Airswitch_Cfg_Write_Type; // cmd 06

struct RetMsg_t // ret message struct
{
  UCHAR retBuf[BUF_MAXLEN]; // ret buffer
  USHORT recvCmd;           // ret addr
  USHORT retLen;            // ret len
} * RetMsg;                 // global save point

eMBErrorCode modbus_airswitch001_init(UCHAR ucPort, ULONG ulBaudRate,
                                      eMBParity eParity, int tx_pin, int rx_pin,
                                      int en_pin);

void mt_modbus_airswitch001_task();

// single api =================================================================
// cmd 01 read addrs
esp_err_t mt_airswitch001_get_addrs(UCHAR slaveAddr, USHORT *addrs);

// cmd 01 read state
esp_err_t mt_airswitch001_get_state(UCHAR slaveAddr, USHORT target,
                                    bool *state);

// cmd 02 read ctrl for each switch
esp_err_t mt_airswitch001_get_ctrl(UCHAR slaveAddr, USHORT target, bool *ctrl);

// cmd 03 get data for switch
esp_err_t mt_airswitch001_get_data(UCHAR slaveAddr,
                                   Airswitch_Data_Read_Type type, UCHAR target,
                                   USHORT *value_out);

// cmd 04 get config for switch
esp_err_t mt_airswitch001_get_config(UCHAR slaveAddr,
                                     Airswitch_Cfg_Read_Type type, UCHAR target,
                                     USHORT *value_out);

// cmd 05 set switch state
esp_err_t mt_airswitch001_set_state(UCHAR slaveAddr, USHORT target, bool state);

// cmd 06 set config for switch
esp_err_t mt_airswitch001_set_config(UCHAR slaveAddr,
                                     Airswitch_Cfg_Write_Type type,
                                     USHORT target, USHORT value);

// complex api ================================================================
esp_err_t mt_airswitch001_get_datas(UCHAR slaveAddr, UCHAR target, bool *state,
                                    bool *ctrl, double *votage,
                                    double *leak_current, double *power,
                                    double *temp, double *current);

esp_err_t mt_airswitch001_get_model(UCHAR slaveAddr, UCHAR target, int *model,
                                    int *current);

#endif