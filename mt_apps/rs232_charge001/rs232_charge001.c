#include "rs232_charge001.h"

#include "mt_utils_string.h"

#include "mt_proto_charge001.pb-c.h"
#include "rs232_charge001_recv_manage.h"
#include "rs232_charge001_recv_manage_get_state.h"
#include "rs232_charge001_recv_manage_get_states.h"
#include "rs232_charge001_recv_manage_set_charge.h"
#include "rs232_charge001_recv_manage_set_stop.h"
#include "rs232_charge001_sent_manage.h"
#include "rs232_charge001_utils.h"
#include "rs232_dev.h"

// static define ==============================================================

static const char *TAG = "RS232_CHARGE001";

static int RS232_CHARGE001_CMD_TIMEOUT = 3000;  // 3s
static int RS232_CHARGE001_LOCK_TIMEOUT = 3000; // 3s
static rs232_dev_config_t *DEV_CONFIG = NULL;
static SemaphoreHandle_t RS232_CHARGE001_LOCK = NULL;

// static func ================================================================

static esp_err_t rs232_charge001_lock_take(long timeout) {
  if (RS232_CHARGE001_LOCK == NULL) {
    RS232_CHARGE001_LOCK = xSemaphoreCreateMutex();
  }
  if (xSemaphoreTake(RS232_CHARGE001_LOCK, (portTickType)timeout) == pdTRUE) {
    return ESP_OK;
  }
  return ESP_ERR_INVALID_RESPONSE;
}

static void rs232_charge001_lock_release() {
  if (RS232_CHARGE001_LOCK == NULL) {
    RS232_CHARGE001_LOCK = xSemaphoreCreateMutex();
  }

  xSemaphoreGive(RS232_CHARGE001_LOCK);
}

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

rs232_charge001_states2_t *rs232_charge001_new_states2(int32_t num) {
  if (num <= 0) {
    ESP_LOGE(TAG, "%4d %s unexcept num:%d", __LINE__, __func__, num);
    return NULL;
  }

  rs232_charge001_states2_t *state2 = malloc(sizeof(rs232_charge001_states2_t));

  state2->num = num;
  state2->states = malloc(num * sizeof(rs232_charge001_state2_t *));

  for (int i = 0; i < num; i++) {
    state2->states[i] = NULL;
  }

  return state2;
}

void rs232_charge001_free_states2(rs232_charge001_states2_t *states2) {
  if (states2 == NULL)
    return;

  for (int i = 0; i < states2->num; i++) {
    if (states2->states[i] != NULL) {
      rs232_charge001_free_state2(states2->states[i]);
    }
  }

  if (states2->states != NULL)
    free(states2->states);

  free(states2);
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
  dev_config->uart_config->stop_bits = UART_STOP_BITS_1;

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
    ESP_LOGW(TAG, "%4d %s DEV_CONFIG exsit", __LINE__, __func__);
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

  // lock
  err = rs232_charge001_lock_take(RS232_CHARGE001_LOCK_TIMEOUT);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_lock_take timeout", __LINE__,
             __func__);
    return NULL;
  }

  // marshal cmd
  buf_sent = rs232_charge001_utils_marshal_get_state(port, &buf_sent_size);
  if (buf_sent == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_utils_marshal_get_state NULL",
             __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
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

  rs232_charge001_lock_release();
  return res;
}

rs232_charge001_states_t *rs232_charge001_get_states() {
  esp_err_t err = ESP_OK;
  uint8_t *buf_sent = NULL;
  int buf_sent_size = 0;
  rs232_charge001_states_t *res = NULL;

  // lock
  err = rs232_charge001_lock_take(RS232_CHARGE001_LOCK_TIMEOUT);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_lock_take timeout", __LINE__,
             __func__);
    return NULL;
  }

  // marshal cmd
  buf_sent = rs232_charge001_utils_marshal_get_states(&buf_sent_size);
  if (buf_sent == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_utils_marshal_get_states NULL",
             __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
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

  rs232_charge001_lock_release();

  return res;
}

esp_err_t rs232_charge001_set_charge(int32_t port, int32_t money, int32_t time,
                                     int32_t *res_port, int32_t *res_result) {
  esp_err_t err = ESP_OK;
  uint8_t *buf_sent = NULL;
  int buf_sent_size = 0;

  // lock
  err = rs232_charge001_lock_take(RS232_CHARGE001_LOCK_TIMEOUT);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_lock_take timeout", __LINE__,
             __func__);
    return err;
  }

  // marshal cmd
  buf_sent = rs232_charge001_utils_marshal_set_charge(port, money, time,
                                                      &buf_sent_size);
  if (buf_sent == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_utils_marshal_set_charge NULL",
             __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
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

  rs232_charge001_lock_release();

  return err;
}

esp_err_t rs232_charge001_set_stop(int32_t port) {
  esp_err_t err = ESP_OK;
  uint8_t *buf_sent = NULL;
  int buf_sent_size = 0;
  int32_t res_port = 0;
  int32_t res_time = 0;

  // lock
  err = rs232_charge001_lock_take(RS232_CHARGE001_LOCK_TIMEOUT);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_lock_take timeout", __LINE__,
             __func__);
    return err;
  }

  // marshal cmd
  buf_sent = rs232_charge001_utils_marshal_set_stop(port, &buf_sent_size);
  if (buf_sent == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_utils_marshal_set_stop NULL",
             __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // reset buf
  rs232_charge001_recv_manage_set_stop_reset();

  // sent cmd
  err = rs232_charge001_sent_manage_sent_and_wait_finish(
      buf_sent, buf_sent_size, RS232_CHARGE001_CMD_TIMEOUT,
      rs232_charge001_recv_manage_set_stop_finish);
  if (err != ESP_OK) {
    ESP_LOGE(TAG,
             "%4d %s rs232_charge001_sent_manage_sent_and_wait_finish failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // get response
  err = rs232_charge001_recv_manage_set_stop_response(&res_port, &res_time);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_recv_manage_set_stop_response failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // parse response
  if (res_port != port) {
    ESP_LOGE(TAG, "%4d %s res_port:%d but port:%d", __LINE__, __func__,
             res_port, port);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

EXIT:
  if (buf_sent != NULL) {
    free(buf_sent);
  }

  rs232_charge001_lock_release();

  return err;
}

// complex global func ========================================================

rs232_charge001_states2_t *rs232_charge001_get_states2() {
  esp_err_t err = ESP_OK;
  rs232_charge001_states_t *states1 = NULL;
  rs232_charge001_states2_t *states2 = NULL;

  states1 = rs232_charge001_get_states();
  if (states1 == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_get_states failed", __LINE__,
             __func__);
    return NULL;
  }

  if (states1->num == 0) {
    ESP_LOGE(TAG, "%4d %s states1->num zero", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  states2 = rs232_charge001_new_states2(states1->num);
  for (int i = 0; i < states1->num; i++) {
    states2->states[i] = rs232_charge001_get_state(states1->states[i]->port);
    if (states2->states[i] == NULL) {
      ESP_LOGE(TAG, "%4d %s rs232_charge001_get_state %d failed", __LINE__,
               __func__, states1->states[i]->port);
      goto EXIT;
    }
  }

EXIT:
  rs232_charge001_free_states(states1);
  if (err != ESP_OK) {
    rs232_charge001_free_states2(states2);
    states2 = NULL;
  }
  return states2;
}

mt_module_flow_struct_group_t *rs232_charge001_get_flow_data() {
  rs232_charge001_states2_t *state2 = rs232_charge001_get_states2();
  char key[24] = "";
  if (state2 == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_get_states2 failed", __LINE__,
             __func__);
    return NULL;
  }

  int one_flow_size = 3;
  mt_module_flow_struct_group_t *data_out =
      mt_module_flow_new_struct_group(state2->num * one_flow_size);
  for (int i = 0; i < state2->num; i++) {
    // state
    sprintf(key, "s%d", i + 1);
    data_out->value[i * one_flow_size]->key = mt_utils_string_copy(key);
    data_out->value[i * one_flow_size]->type =
        GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
    data_out->value[i * one_flow_size]->number_value =
        state2->states[i]->state * 1.0;

    // lefttime
    sprintf(key, "t%d", i + 1);
    data_out->value[i * one_flow_size + 1]->key = mt_utils_string_copy(key);
    data_out->value[i * one_flow_size + 1]->type =
        GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
    data_out->value[i * one_flow_size + 1]->number_value =
        state2->states[i]->lefttime * 1.0;

    // power
    sprintf(key, "p%d", i + 1);
    data_out->value[i * one_flow_size + 2]->key = mt_utils_string_copy(key);
    data_out->value[i * one_flow_size + 2]->type =
        GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
    data_out->value[i * one_flow_size + 2]->number_value =
        state2->states[i]->power;
  }

  rs232_charge001_free_states2(state2);
  return data_out;
}

mt_module_flow_struct_group_t *
rs232_charge001_get_port_notify_data(int32_t port) {
  rs232_charge001_state2_t *state2 = rs232_charge001_get_state(port);
  int count = 0;
  char key[24] = "";
  if (state2 == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_get_state port:%d failed", __LINE__,
             __func__, port);
    return NULL;
  }

  mt_module_flow_struct_group_t *data_out = mt_module_flow_new_struct_group(4);

  // notify
  data_out->value[count]->key = mt_utils_string_copy("notify");
  data_out->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE;
  data_out->value[count++]->bool_value = true;

  // state
  sprintf(key, "s%d", port);
  data_out->value[count]->key = mt_utils_string_copy(key);
  data_out->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  data_out->value[count++]->number_value = state2->state;

  // lefttime
  sprintf(key, "t%d", port);
  data_out->value[count]->key = mt_utils_string_copy(key);
  data_out->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  data_out->value[count++]->number_value = state2->lefttime;

  // power
  sprintf(key, "p%d", port);
  data_out->value[count]->key = mt_utils_string_copy(key);
  data_out->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  data_out->value[count++]->number_value = state2->power;

  rs232_charge001_free_state2(state2);
  return data_out;
}
