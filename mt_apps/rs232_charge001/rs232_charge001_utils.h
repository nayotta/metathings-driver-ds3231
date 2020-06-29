#ifndef _RS232_CHARGE001_UTILS_H
#define _RS232_CHARGE001_UTILS_H

#include "esp_err.h"
#include "esp_log.h"
#include "string.h"

// define =====================================================================

typedef struct _rs232_charge001_msg_t {
  uint8_t size;
  uint8_t cmd;
  uint64_t session;
  uint8_t *buf;
} rs232_charge001_msg_t;

// help func ==================================================================

rs232_charge001_msg_t *rs232_charge001_utils_new_msg();

void rs232_charge001_utils_free_msg(rs232_charge001_msg_t *msg);

// func =======================================================================

uint8_t *rs232_charge001_utils_marshal_buf(uint8_t cmd, uint8_t *data,
                                           int data_size, int *buf_size_out);

rs232_charge001_msg_t *rs232_chrage001_utils_unmarshal_buf(uint8_t *buf,
                                                           int32_t size);

uint8_t *rs232_charge001_utils_marshal_set_charge(int32_t port, int32_t money,
                                                  int32_t time,
                                                  int32_t *buf_size);

uint8_t *rs232_charge001_utils_marshal_get_state(int32_t port,
                                                 int32_t *buf_size);

uint8_t *rs232_charge001_utils_marshal_get_states(int32_t *buf_size);

#endif
