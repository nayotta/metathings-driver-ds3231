#include "rs232_charge001_utils.h"

// static define ==============================================================

static const char *TAG = "RS232_CHARGE001_UTILS";

// help func ==================================================================

rs232_charge001_msg_t *rs232_charge001_utils_new_msg() {
  rs232_charge001_msg_t *msg = malloc(sizeof(rs232_charge001_msg_t));

  msg->size = 0;
  msg->cmd = 0;
  msg->session = 0;
  msg->buf = NULL;

  return msg;
}

void rs232_charge001_utils_free_msg(rs232_charge001_msg_t *msg) {
  if (msg == NULL)
    return;

  if (msg->buf != NULL)
    free(msg->buf);

  free(msg);
}

static uint8_t rs232_charge001_sum(uint8_t *buf, int32_t size) {
  uint8_t sum = 0;

  for (int i = 1; i < size - 1; i++) {
    sum = sum ^ buf[i];
  }

  return sum;
}

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
  uint8_t sum = rs232_charge001_sum(buf_out, buf_out_size);

  buf_out[buf_out_size - 1] = sum;

  *buf_size_out = buf_out_size;

  return buf_out;
}

rs232_charge001_msg_t *rs232_chrage001_utils_unmarshal_buf(uint8_t *buf,
                                                           int32_t size) {
  esp_err_t err = ESP_OK;
  rs232_charge001_msg_t *msg = rs232_charge001_utils_new_msg();

  if (size <= 10) {
    ESP_LOGE(TAG, "%4d %s size:%d error", __LINE__, __func__, size);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // check head
  if (!((buf[0] == 0xEE) || (buf[0] == 0x66))) {
    ESP_LOGE(TAG, "%4d %s buf[0]:%2x not equal 0xEE or 0x66", __LINE__,
             __func__, buf[0]);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // check min size
  if (buf[1] - 8 <= 0) {
    ESP_LOGE(TAG, "%4d %s buf[1]:%2x less than 8", __LINE__, __func__, buf[1]);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // save buf size
  msg->size = buf[1] - 8;

  // save cmd
  msg->cmd = buf[2];

  // save session
  memcpy(&msg->session, buf + 3, 6);

  // save buf
  msg->buf = malloc(msg->size);
  memcpy(msg->buf, buf + 9, msg->size);

  // check sum
  uint8_t sum = rs232_charge001_sum(buf, size);
  if (sum != buf[size - 1]) {
    ESP_LOGE(TAG, "%4d %s except sum:%2x recv:%2x", __LINE__, __func__, sum,
             buf[size - 1]);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

EXIT:
  if (err != ESP_OK) {
    rs232_charge001_utils_free_msg(msg);
    msg = NULL;
  }

  return msg;
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
