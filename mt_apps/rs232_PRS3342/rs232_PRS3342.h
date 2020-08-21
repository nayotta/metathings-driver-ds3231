#ifndef _RS232_RPS3342_H
#define _RS232_RPS3342_H

#include "esp_err.h"
#include "rs232_dev.h"

#define RS232_PRS3342_DATA_MIN_SIZE 18

typedef enum {
  RS232_PRS3342_DATA_TYPE_GUIYI = 0x09,
  RS232_PRS3342_DATA_TYPE_SHORT_FLOAT,
  RS232_PRS3342_DATA_TYPE_LONG_FLOAT,
  RS232_PRS3342_DATA_TYPE_BOOL = 0x01,
} rs232_PRS342_data_type_t;

typedef struct {
  int addr;
  char *name;
  float convert;
  rs232_PRS342_data_type_t type;

  union {
    float data;
    bool state;
    int32_t number_data;
  };

  int64_t update_time;
} rs232_PRS3342_data_t;

typedef struct {
  rs232_dev_config_t *rs232_config;
  int data_num;
  rs232_PRS3342_data_t **datas;
} rs232_PRS3342_t;

esp_err_t rs232_PRS3342_task(rs232_PRS3342_t *prs3342);

esp_err_t rs232_PRS3342_new_guiyi(rs232_PRS3342_data_t *data, int addr,
                                  float convert, char *name);

esp_err_t rs232_PRS3342_new_int(rs232_PRS3342_data_t *data, int addr,
                                  float convert, char *name);

esp_err_t rs232_PRS3342_set_context(rs232_PRS3342_t *prs3342);

rs232_PRS3342_t *rs232_PRS3342_get_context();

#endif
