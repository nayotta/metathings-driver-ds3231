#include "rs232_charge001_utils.h"

// static define ==============================================================

// static const char *TAG = "RS232_CHARGE001_UTILS";

// global func ================================================================

uint8_t *rs232_charge001_utils_marshal_buf(uint8_t cmd, uint8_t *data,
                                           int data_size, int *buf_size_out) {
  uint8_t *buf_out = NULL;
  int buf_out_size = 1 + 1 + 1 + 6 + data_size + 1;

  buf_out = malloc(buf_out_size);

  // gen head
  buf_out[0] = 0xEE;

  // gen len
  buf_out[1] = buf_out_size - 2;

  // gen cmd
  buf_out[2] = (char)cmd;

  // gen session
  buf_out[3] = 0;
  buf_out[4] = 0;
  buf_out[5] = 0;
  buf_out[6] = 0;
  buf_out[7] = 0;
  buf_out[8] = 0;

  // gen data
  memcpy(buf_out + 9, data, data_size);

  // gen sum
  uint8_t sum = 0;
  for (int i = 1; i < buf_out_size - 1; i++) {
    sum = sum ^ buf_out[i];
  }
  buf_out[buf_out_size - 1] = sum;

  *buf_size_out = buf_out_size;

  return buf_out;
}

uint8_t *rs232_charge001_utils_marshal_set_charge(int32_t port, int32_t money,
                                                  int32_t time,
                                                  int32_t *buf_size) {
  uint8_t *buf_out = NULL;
  uint8_t data[5] = "";
  int data_size = 5;

  data[0] = port & 0x00ff;
  data[1] = money >> 8;
  data[2] = money & 0x00ff;
  data[3] = time >> 8;
  data[4] = time & 0x00ff;

  buf_out = rs232_charge001_utils_marshal_buf(2, data, data_size, buf_size);

  return buf_out;
}

uint8_t *rs232_charge001_utils_marshal_get_state(int32_t port,
                                                 int32_t *buf_size) {
  uint8_t *buf_out = NULL;
  uint8_t data[1] = "";
  int data_size = 1;

  data[0] = port & 0x000000ff;

  buf_out = rs232_charge001_utils_marshal_buf(6, data, data_size, buf_size);

  return buf_out;
}

uint8_t *rs232_charge001_utils_marshal_get_states(int32_t *buf_size) {
  uint8_t *buf_out = NULL;
  uint8_t data[1] = "";
  int data_size = 1;

  data[0] = 0x00;

  buf_out = rs232_charge001_utils_marshal_buf(1, data, data_size, buf_size);

  return buf_out;
}
