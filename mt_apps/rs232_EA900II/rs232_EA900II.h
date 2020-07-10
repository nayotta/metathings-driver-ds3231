#ifndef _RS232_EA900II_H
#define _RS232_EA900II_H

#include "esp_err.h"
#include "rs232_dev.h"
#include "stdio.h"

typedef struct _rs232_EA900II_status_t {
  double vIn;    // 输入电压
  double vOut;   // 输出电压
  double load;   // 负载百分比
  double freqIn; // 输入频率
  double vBat;   // 电池电压
  double temp;   // 电池温度
  bool w7;       // 市电故障
  bool w6;       // 电池电压低
  bool w5;       // 旁路/逆变状态
  bool w4;       // 内不故障
  bool w3;       // ups 类型
  bool w2;       // 系统测试状态
  bool w1;       // 系统关机状态
  bool w0;       // 告警音
} rs232_EA900II_status_t;

typedef struct _rs232_EA900II_model_t {
  char name[16];    // 公司名称
  char model[11];   // UPS型号
  char version[11]; // UPS版本
} rs232_EA900II_model_t;

typedef struct _rs232_EA900II_config_t {
  double votage;    // 额定电压
  double current;   // 额定电流
  double batVotage; // 电池最大电压
  double freq;      // 额定频率
} rs232_EA900II_config_t;

rs232_EA900II_status_t *rs232_EA900II_get_status();

rs232_EA900II_model_t *rs232_EA900II_get_model();

rs232_EA900II_config_t *rs232_EA900II_get_config();

esp_err_t rs232_EA900II_task(int tx_pin, int rx_pin);

#endif
