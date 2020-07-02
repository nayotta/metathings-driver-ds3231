#include "esp_err.h"
#include "esp_log.h"

#include "google/protobuf/any.pb-c.h"
#include "google/protobuf/empty.pb-c.h"
#include "mt_proto_charge001.pb-c.h"
#include "stream_frame.pb-c.h"

#include "mt_module_flow.h"
#include "mt_module_flow_manage.h"
#include "mt_module_mqtt.h"
#include "mt_module_unarycall_utils.h"
#include "mt_mqtt_lan.h"
#include "mt_mqtt_utils.h"
#include "mt_utils_error.h"

#include "rs232_charge001.h"
#include "rs232_charge001_utils.h"

// global config ==============================================================

static const char *TAG = "rs232_charge001_module_mqtt";

// static  func ===============================================================

static void rs232_charge001_mqtt_process_set_charge(
    Ai__Metathings__Component__DownStreamFrame *msg, char *module_id) {
  esp_err_t err = ESP_OK;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  MtCharge001__SetChargeReq *req = NULL;
  MtCharge001__SetRes res = MT_CHARGE001__SET_RES__INIT;
  res.code = MT_ERR_NO_ERR;

  // arg checks
  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.code = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // get req
  req = mt_charge001__set_charge_req__unpack(
      NULL, msg->unary_call->value->value.len,
      msg->unary_call->value->value.data);
  err = rs232_charge001_utils_check_set_charge_req(req);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_utils_check_set_charge_req failed",
             __LINE__, __func__);
    res.code = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // set charge
  ESP_LOGI(TAG, "%4d %s set_charge port:%d time:%d money:%d", __LINE__,
           __func__, req->charge->port, req->charge->time, req->charge->money);
  int32_t result = 0;
  int32_t res_port = 0;
  err = rs232_charge001_set_charge(req->charge->port, req->charge->money,
                                   req->charge->time, &res_port, &result);
  if (err != ESP_OK || (req->charge->port != res_port)) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_set_charge failed", __LINE__,
             __func__);
    res.code = MT_ERR_INVALID_RESPONSE;
    goto ERROR;
  }

ERROR:
  // response
  res_size = mt_charge001__set_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_charge001__set_res__pack(&res, res_buf);
  err = mt_module_unarycall_utils_mqtt_sent_msg(
      res_buf, res_size, module_id, mt_charge001__set_res__descriptor.name,
      msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == MT_ERR_NO_ERR) {
    ESP_LOGI(TAG, "%4d %s set_charge success", __LINE__, __func__);
  } else {
    ESP_LOGE(TAG, "%4d %s set_charge failed", __LINE__, __func__);
  }

EXIT:
  if (req != NULL)
    mt_charge001__set_charge_req__free_unpacked(req, NULL);
  if (res_buf != NULL)
    free(res_buf);
  return;
}

static void rs232_charge001_mqtt_process_get_states(
    Ai__Metathings__Component__DownStreamFrame *msg, char *module_id) {
  esp_err_t err = ESP_OK;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  MtCharge001__GetStatesRes *res = malloc(sizeof(MtCharge001__GetStatesRes));
  mt_charge001__get_states_res__init(res);
  res->code = MT_ERR_NO_ERR;
  rs232_charge001_states2_t *state = NULL;

  // arg checks
  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res->code = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // get data process
  state = rs232_charge001_get_states2();
  if (state == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_get_states2 failed", __LINE__,
             __func__);
    res->code = MT_ERR_INVALID_RESPONSE;
    goto ERROR;
  }
  res->portnum = state->num;
  res->n_states = state->num;
  res->states = malloc(state->num * sizeof(MtCharge001__State2 *));
  for (int i = 0; i < state->num; i++) {
    res->states[i] = malloc(sizeof(MtCharge001__State2));
    res->states[i]->port = state->states[i]->port;
    res->states[i]->state = state->states[i]->state;
    res->states[i]->lefttime = state->states[i]->lefttime;
    res->states[i]->power = state->states[i]->power;
  }

ERROR:
  // response
  res_size = mt_charge001__get_states_res__get_packed_size(res);
  res_buf = malloc(res_size);
  mt_charge001__get_states_res__pack(res, res_buf);
  err = mt_module_unarycall_utils_mqtt_sent_msg(
      res_buf, res_size, module_id,
      mt_charge001__get_states_res__descriptor.name, msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res->code == MT_ERR_NO_ERR) {
    ESP_LOGI(TAG, "%4d %s GetStates success", __LINE__, __func__);
    for (int i = 0; i < state->num; i++) {
      ESP_LOGI(TAG, "%4d %s port:%d state:%d lefttime:%d power:%d", __LINE__,
               __func__, state->states[i]->port, state->states[i]->state,
               state->states[i]->lefttime, state->states[i]->power);
    }
  } else {
    ESP_LOGE(TAG, "%4d %s GetStates failed", __LINE__, __func__);
  }

EXIT:
  rs232_charge001_free_states2(state);
  mt_charge001__get_states_res__free_unpacked(res, NULL);
  if (res_buf != NULL)
    free(res_buf);
  return;
}

static void rs232_charge001_mqtt_process_get_state(
    Ai__Metathings__Component__DownStreamFrame *msg, char *module_id) {
  esp_err_t err = ESP_OK;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  MtCharge001__GetStateReq *req = NULL;
  MtCharge001__GetStateRes res = MT_CHARGE001__GET_STATE_RES__INIT;
  res.code = MT_ERR_NO_ERR;
  rs232_charge001_state2_t *state = NULL;

  // arg checks
  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.code = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // get req
  req = mt_charge001__get_state_req__unpack(NULL,
                                            msg->unary_call->value->value.len,
                                            msg->unary_call->value->value.data);
  err = rs232_charge001_utils_check_get_state_req(req);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_utils_check_get_state_req failed",
             __LINE__, __func__);
    res.code = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // get state
  state = rs232_charge001_get_state(req->port);
  if (state == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_get_state failed", __LINE__,
             __func__);
    res.code = MT_ERR_INVALID_RESPONSE;
    goto ERROR;
  }

  ESP_LOGI(TAG, "%4d %s get_state port:%d state:%d lefttime:%d power:%d",
           __LINE__, __func__, state->port, state->state, state->lefttime,
           state->power);

ERROR:
  // response
  res_size = mt_charge001__get_state_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_charge001__get_state_res__pack(&res, res_buf);
  err = mt_module_unarycall_utils_mqtt_sent_msg(
      res_buf, res_size, module_id,
      mt_charge001__get_state_res__descriptor.name, msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == MT_ERR_NO_ERR) {
    ESP_LOGI(TAG, "%4d %s set_charge success", __LINE__, __func__);
  } else {
    ESP_LOGE(TAG, "%4d %s set_charge failed", __LINE__, __func__);
  }

EXIT:
  if (req != NULL)
    mt_charge001__get_state_req__free_unpacked(req, NULL);
  rs232_charge001_free_state2(state);
  if (res_buf != NULL)
    free(res_buf);
  return;
}

// global func ================================================================

void rs232_charge001_module_mqtt_init() {
  mt_module_mqtt_add_handle(rs232_charge001_mqtt_process_set_charge,
                            "SetCharge");
  mt_module_mqtt_add_handle(rs232_charge001_mqtt_process_get_states,
                            "GetStates");
  mt_module_mqtt_add_handle(rs232_charge001_mqtt_process_get_state, "GetState");
}
