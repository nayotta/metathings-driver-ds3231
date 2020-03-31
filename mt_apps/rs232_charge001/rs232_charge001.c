#include "rs232_charge001.h"

#include "rs232_dev.h"

// static define ==============================================================

static const char *TAG = "RS232_CHARGE001";

static int RS232_CHARGE001_CMD_TIMEOUT = 3000; // 3s
static rs232_dev_config_t *DEV_CONFIG = NULL;

// help func ==================================================================

rs232_charge001_state1_t *rs232_charge001_new_state1() {
  rs232_charge001_state1_t *state = malloc(sizeof(rs232_charge001_state1_t));

  state->state = 0;
  state->port = 0;

  return state;
}

void rs232_charge001_free_state1(rs232_charge001_state1_t *state) {
  if (state == NULL)
    return;

  free(state);
}

rs232_charge001_state2_t *rs232_charge001_new_state2() {
  rs232_charge001_state2_t *state = malloc(sizeof(rs232_charge001_state2_t));

  state->state = 0;
  state->lefttime = 0;
  state->port = 0;
  state->power = 0;

  return state;
}

void rs232_charge001_free_state2(rs232_charge001_state2_t *state) {
  if (state == NULL)
    return;

  free(state);
}

rs232_charge001_states_t *rs232_charge001_new_states(int32_t num) {
  if (num <= 0) {
    ESP_LOGE(TAG, "%4d %s unexcept num:%d", __LINE__, __func__, num);
    return NULL;
  }

  rs232_charge001_states_t *state = malloc(sizeof(rs232_charge001_states_t));

  state->num = num;
  state->states = malloc(num * sizeof(rs232_charge001_state1_t *));

  for (int i = 0; i < num; i++) {
    state->states[i] = NULL;
  }

  return state;
}

void rs232_charge001_free_states(rs232_charge001_states_t *states) {
  if (states == NULL)
    return;

  for (int i = 0; i < states->num; i++) {
    if (states->states[i] != NULL) {
      rs232_charge001_free_state1(states->states[i]);
    }
  }

  if (states->states != NULL)
    free(states->states);

  free(states);
}

// global func ================================================================

esp_err_t rs232_charge001_init(int32_t uart_num, int32_t rx_pin,
                               int32_t tx_pin) {
  esp_err_t err = ESP_OK;
  rs232_dev_config_t *dev_config = rs232_dev_default_new();

  dev_config->uart_num = uart_num;
  dev_config->rx_pin = rx_pin;
  dev_config->tx_pin = tx_pin;
  dev_config->uart_config->baud_rate = 9600;
  dev_config->uart_config->data_bits = UART_STOP_BITS_2;

  err = rs232_dev_init(dev_config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_dev_init failed", __LINE__, __func__);
    return err;
  }

  err = rs232_charge001_sent_manage_init(dev_config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_sent_manage_init failed", __LINE__,
             __func__);
    return err;
  }

  err = rs232_charge001_recv_manage_init(dev_config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_recv_manage_init failed", __LINE__,
             __func__);
    return err;
  }

  if (DEV_CONFIG != NULL) {
    rs232_dev_config_free(DEV_CONFIG);
  }

  DEV_CONFIG = dev_config;

  ESP_LOGI(TAG, "%4d %s init success", __LINE__, __func__);

  return ESP_OK;
}

rs232_charge001_state2_t *rs232_charge001_get_state(int32_t port) {
  esp_err_t err = ESP_OK;
  uint8_t *buf_sent = NULL;
  int buf_sent_size = 0;
  rs232_charge001_state2_t *res = NULL;

  // marshal cmd
  buf_sent = rs232_charge001_utils_marshal_get_state(port, &buf_sent_size);
  if (buf_sent == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_utils_marshal_get_state NULL",
             __LINE__, __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // reset buf
  rs232_charge001_recv_manage_get_state_reset();

  // sent cmd
  err = rs232_charge001_sent_manage_sent_and_wait_finish(
      buf_sent, buf_sent_size, RS232_CHARGE001_CMD_TIMEOUT,
      rs232_charge001_recv_manage_get_state_finish);
  if (err != ESP_OK) {
    ESP_LOGE(TAG,
             "%4d %s rs232_charge001_sent_manage_sent_and_wait_finish failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // get response
  res = rs232_charge001_recv_manage_get_state_response();
  if (res == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_recv_manage_get_state_response NULL",
             __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // parse response
  if (res->port != port) {
    ESP_LOGE(TAG, "%4d %s res_port:%d but port:%d", __LINE__, __func__,
             res->port, port);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

EXIT:
  if (buf_sent != NULL) {
    free(buf_sent);
  }

  if (err != ESP_OK) {
    if (res != NULL)
      rs232_charge001_free_state2(res);
    res = NULL;
  }

  return res;
}

rs232_charge001_states_t *rs232_charge001_get_states() {
  esp_err_t err = ESP_OK;
  uint8_t *buf_sent = NULL;
  int buf_sent_size = 0;
  rs232_charge001_states_t *res = NULL;

  // marshal cmd
  buf_sent = rs232_charge001_utils_marshal_get_states(&buf_sent_size);
  if (buf_sent == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_utils_marshal_get_states NULL",
             __LINE__, __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // reset buf
  rs232_charge001_recv_manage_get_states_reset();

  // sent cmd
  err = rs232_charge001_sent_manage_sent_and_wait_finish(
      buf_sent, buf_sent_size, RS232_CHARGE001_CMD_TIMEOUT,
      rs232_charge001_recv_manage_get_states_finish);
  if (err != ESP_OK) {
    ESP_LOGE(TAG,
             "%4d %s rs232_charge001_sent_manage_sent_and_wait_finish failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // get response
  res = rs232_charge001_recv_manage_get_states_response();
  if (res == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_recv_manage_get_state_response NULL",
             __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // parse response
  if (res->num <= 0) {
    ESP_LOGE(TAG, "%4d %s port_num:%d error", __LINE__, __func__, res->num);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

EXIT:
  if (buf_sent != NULL) {
    free(buf_sent);
  }

  if (err != ESP_OK) {
    if (res != NULL)
      rs232_charge001_free_states(res);
    res = NULL;
  }

  return res;
}

esp_err_t rs232_charge001_set_charge(int32_t port, int32_t money, int32_t time,
                                     int32_t *res_port, int32_t *res_result) {
  esp_err_t err = ESP_OK;
  uint8_t *buf_sent = NULL;
  int buf_sent_size = 0;
  int32_t res_port = 0;
  int32_t res_result = 0;

  // marshal cmd
  buf_sent = rs232_charge001_utils_marshal_set_charge(port, money, time,
                                                      &buf_sent_size);
  if (buf_sent == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_utils_marshal_set_charge NULL",
             __LINE__, __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // reset buf
  rs232_charge001_recv_manage_set_charge_reset();

  // sent cmd
  err = rs232_charge001_sent_manage_sent_and_wait_finish(
      buf_sent, buf_sent_size, RS232_CHARGE001_CMD_TIMEOUT,
      rs232_charge001_recv_manage_set_charge_finish);
  if (err != ESP_OK) {
    ESP_LOGE(TAG,
             "%4d %s rs232_charge001_sent_manage_sent_and_wait_finish failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // get response
  err = rs232_charge001_recv_manage_set_charge_response(res_port, res_result);
  if (err != ESP_OK) {
    ESP_LOGE(TAG,
             "%4d %s rs232_charge001_recv_manage_set_charge_response failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // parse response
  if (*res_port != port) {
    ESP_LOGE(TAG, "%4d %s res_port:%d but port:%d", __LINE__, __func__,
             *res_port, port);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

EXIT:
  if (buf_sent != NULL) {
    free(buf_sent);
  }

  return err;
}
