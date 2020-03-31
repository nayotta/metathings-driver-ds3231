#include "rs232_charge001_recv_manage_get_states.h"

// static define ==============================================================

static const char *TAG = "rs232_charge001_recv_manage_get_states";
static bool STATE = false;
#define CHARGE001_GET_STATES_BUF_MAX_SIZE (1024)
static uint8_t BUF[CHARGE001_GET_STATES_BUF_MAX_SIZE] = "";
static int32_t BUF_SIZE = 0;

// help func ==================================================================

void rs232_charge001_recv_manage_get_states_reset() {
  STATE = false;
  BUF[0] = '\0';
  BUF_SIZE = 0;
}

// global func ================================================================

esp_err_t rs232_charge001_recv_manage_get_states_parse(uint8_t *buf,
                                                       int32_t size) {

  if (size >= CHARGE001_GET_STATES_BUF_MAX_SIZE) {
    ESP_LOGE(TAG, "%4d %s size:%d bigger than:%d", __LINE__, __func__, size,
             CHARGE001_GET_STATES_BUF_MAX_SIZE);
    return ESP_ERR_INVALID_ARG;
  }

  memcpy(BUF, buf, size);
  BUF_SIZE = size;
  STATE = true;

  return ESP_OK;
}

esp_err_t rs232_charge001_recv_manage_get_states_finish() {
  if (STATE == true) {
    return ESP_OK;
  } else {
    return ESP_ERR_INVALID_RESPONSE;
  }
}

rs232_charge001_states_t *rs232_charge001_recv_manage_get_states_response() {
  rs232_charge001_states_t *res = NULL;
  esp_err_t err = ESP_OK;
  uint8_t *temp_buf = NULL;
  int32_t temp_buf_size = 0;
  int32_t port_num = 0;

  temp_buf_size = BUF_SIZE;
  if (temp_buf_size > 0) {
    temp_buf = malloc(temp_buf_size);
    memcpy(temp_buf, BUF, temp_buf_size);
  } else {
    ESP_LOGE(TAG, "%4d %s BUF_SIZE:%d error", __LINE__, __func__, BUF_SIZE);
    return ESP_ERR_INVALID_ARG;
  }

  port_num = temp_buf[0];
  if (temp_buf_size - 1 != port_num) {
    ESP_LOGE(TAG, "%4d %s except:%d but get:%d", __LINE__, __func__, port_num,
             temp_buf_size - 1);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  res = rs232_charge001_new_states(port_num);
  if (res == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_new_states failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  for (int i = 0; i < port_num; i++) {
    res->states[i] = malloc(sizeof(rs232_charge001_state1_t));
    res->states[i]->port = i + 1;
    res->states[i]->state = temp_buf[i + 1];
  }

EXIT:
  if (temp_buf != NULL)
    free(temp_buf);

  if (err != ESP_OK) {
    rs232_charge001_free_states(res);
    res = NULL;
  }

  return res;
}
