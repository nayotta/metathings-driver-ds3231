#include "rs232_charge001_recv_manage_get_state.h"

// static define ==============================================================

static const char *TAG = "rs232_charge001_recv_manage_get_state";
static bool STATE = false;
#define CHARGE001_GET_STATE_BUF_MAX_SIZE (1024)
static uint8_t BUF[CHARGE001_GET_STATE_BUF_MAX_SIZE] = "";
static int32_t BUF_SIZE = 0;
static int32_t STATE_RES_SIZE = 5;

// help func ==================================================================

void rs232_charge001_recv_manage_get_state_reset() {
  STATE = false;
  BUF[0] = '\0';
  BUF_SIZE = 0;
}

// global func ================================================================

esp_err_t rs232_charge001_recv_manage_get_state_parse(uint8_t *buf,
                                                      int32_t size) {

  if (size >= CHARGE001_GET_STATE_BUF_MAX_SIZE) {
    ESP_LOGE(TAG, "%4d %s size:%d bigger than:%d", __LINE__, __func__, size,
             CHARGE001_GET_STATE_BUF_MAX_SIZE);
    return ESP_ERR_INVALID_ARG;
  }

  memcpy(BUF, buf, size);
  BUF_SIZE = size;
  STATE = true;

  return ESP_OK;
}

esp_err_t rs232_charge001_recv_manage_get_state_finish() {
  if (STATE == true) {
    return ESP_OK;
  } else {
    return ESP_ERR_INVALID_RESPONSE;
  }
}

rs232_charge001_state2_t *rs232_charge001_recv_manage_get_state_response() {
  rs232_charge001_state2_t *res = NULL;
  uint8_t *temp_buf = NULL;
  int32_t temp_buf_size = 0;

  temp_buf_size = BUF_SIZE;
  if (temp_buf_size > 0) {
    temp_buf = malloc(temp_buf_size);
    memcpy(temp_buf, BUF, temp_buf_size);
  } else {
    ESP_LOGE(TAG, "%4d %s BUF_SIZE:%d error", __LINE__, __func__, BUF_SIZE);
    return NULL;
  }

  if (temp_buf_size != STATE_RES_SIZE) {
    ESP_LOGE(TAG, "%4d %s except size:%d get:%d", __LINE__, __func__,
             STATE_RES_SIZE, temp_buf_size);
    goto EXIT;
  }

  res = malloc(sizeof(rs232_charge001_state2_t));
  res->port = temp_buf[0];
  res->lefttime = temp_buf[1] * 0xFF + temp_buf[2];
  res->power = (temp_buf[3] * 0xFF + temp_buf[4]) / 10;
  if (res->lefttime == 0) {
    res->state = 1;
  } else {
    res->state = 2;
  }

EXIT:
  if (temp_buf != NULL)
    free(temp_buf);

  return res;
}
