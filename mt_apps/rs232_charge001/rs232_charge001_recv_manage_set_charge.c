#include "rs232_charge001_recv_manage_set_charge.h"

// static define ==============================================================

static const char *TAG = "rs232_charge001_recv_manage_set_charge";
static bool STATE = false;
#define CHARGE001_SET_CHARGE_BUF_MAX_SIZE (1024)
static uint8_t BUF[CHARGE001_SET_CHARGE_BUF_MAX_SIZE] = "";
static int32_t BUF_SIZE = 0;
static int32_t STATE_RES_SIZE = 2;

// help func ==================================================================

void rs232_charge001_recv_manage_set_charge_reset() {
  STATE = false;
  BUF[0] = '\0';
  BUF_SIZE = 0;
}

// global func ================================================================

esp_err_t rs232_charge001_recv_manage_set_charge_parse(uint8_t *buf,
                                                       int32_t size) {

  if (size >= CHARGE001_SET_CHARGE_BUF_MAX_SIZE) {
    ESP_LOGE(TAG, "%4d %s size:%d bigger than:%d", __LINE__, __func__, size,
             CHARGE001_SET_CHARGE_BUF_MAX_SIZE);
    return ESP_ERR_INVALID_ARG;
  }

  memcpy(BUF, buf, size);
  BUF_SIZE = size;
  STATE = true;

  return ESP_OK;
}

esp_err_t rs232_charge001_recv_manage_set_charge_finish() {
  if (STATE == true) {
    return ESP_OK;
  } else {
    return ESP_ERR_INVALID_RESPONSE;
  }
}

esp_err_t rs232_charge001_recv_manage_set_charge_response(int32_t *port,
                                                          int32_t *result) {
  esp_err_t err = ESP_OK;
  uint8_t *temp_buf = NULL;
  int32_t temp_buf_size = 0;

  temp_buf_size = BUF_SIZE;
  if (temp_buf_size > 0) {
    temp_buf = malloc(temp_buf_size);
    memcpy(temp_buf, BUF, temp_buf_size);
  } else {
    ESP_LOGE(TAG, "%4d %s BUF_SIZE:%d error", __LINE__, __func__, BUF_SIZE);
    return ESP_ERR_INVALID_ARG;
  }

  if (temp_buf_size != STATE_RES_SIZE) {
    ESP_LOGE(TAG, "%4d %s except size:%d get:%d", __LINE__, __func__,
             STATE_RES_SIZE, temp_buf_size);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  *port = temp_buf[0];
  *result = temp_buf[1];

EXIT:
  if (temp_buf != NULL)
    free(temp_buf);

  return err;
}
