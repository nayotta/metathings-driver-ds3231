#ifndef _RS232_EA900II_H
#define _RS232_EA900II_H

#include "esp_err.h"
#include "rs232_dev.h"
#include "stdio.h"

typedef enum
{
  RS232_LORA_JXYL001_TYPE_UNKNOWN,
  RS232_LORA_JXYL001_TYPE_YL102,
  RS232_LORA_JXYL001_TYPE_YL103
} Rs232_Lora_Jxyl001_Type;

typedef struct _rs232_lora_jxyl001_t
{
  uint32_t addr;
  Rs232_Lora_Jxyl001_Type type; // 设备类型
  int64_t update_time;         // 数据更新时间
  double yl102_votage;          // 烟雾传感器 电压
  bool yl102_fog;               // 烟雾传感器 状态
  double yl103_votage;          // 温湿度传感器 电压
  double yl103_temp;            // 温湿度传感器 温度
  double yl103_hum;             // 温湿度传感器 湿度
} Rs232_Lora_Jxyl001_t;

typedef struct _rs232_lora_jxyl001_devs
{
  rs232_dev_config_t *rs232_config;
  int dev_num;
  Rs232_Lora_Jxyl001_t **devs;
} Rs232_Lora_Jxyl001_Devs;

Rs232_Lora_Jxyl001_t *rs232_lora_jxyl001_default_new(
    uint32_t addr, Rs232_Lora_Jxyl001_Type type);

Rs232_Lora_Jxyl001_Devs *rs232_lora_jxyl001_devs_default_new(int dev_num);

esp_err_t rs232_lora_jxyl001_task(Rs232_Lora_Jxyl001_Devs *devs);

#endif
