#ifndef _RS232_CHARGE001_UTILS_H
#define _RS232_CHARGE001_UTILS_H

#include "esp_err.h"
#include "esp_log.h"
#include "string.h"

// func =======================================================================

uint8_t *rs232_charge001_utils_marshal_buf(uint8_t cmd, uint8_t *data,
                                           int data_size, int *buf_size_out);

uint8_t *rs232_charge001_utils_marshal_set_charge(int32_t port, int32_t money,
                                                  int32_t time,
                                                  int32_t *buf_size);

uint8_t *rs232_charge001_utils_marshal_get_state(int32_t port,
                                                 int32_t *buf_size);

uint8_t *rs232_charge001_utils_marshal_get_states(int32_t *buf_size);

#endif
