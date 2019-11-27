#ifndef _RS232_LORA_JXYL001_H
#define _RS232_LORA_JXYL001_H

#include "esp_err.h"
#include "rs232_dev.h"
#include "stdio.h"

typedef struct _rs232_lora_jxyl001_fog
{
  int32_t addr;
  int64_t update_time;
  double votage;
  bool fog;
} rs232_lora_jxyl001_fog;

typedef struct _rs232_lora_jxyl001_temp
{
  int32_t addr;
  int64_t update_time;
  double votage;
  double temp;
  double hum;
} rs232_lora_jxyl001_temp;

typedef struct _rs232_lora_jxyl001_devs
{
  rs232_dev_config_t *rs232_config;
  int32_t fog_num;
  rs232_lora_jxyl001_fog **fogs;
  int32_t temp_num;
  rs232_lora_jxyl001_temp **temps;
} Rs232_Lora_Jxyl001_Devs;

esp_err_t rs232_lora_jxyl001_task();

#endif
