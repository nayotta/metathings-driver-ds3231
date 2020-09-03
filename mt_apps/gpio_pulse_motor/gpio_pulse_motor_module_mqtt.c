#include "stdio.h"
#include "string.h"

#include "esp_err.h"
#include "esp_log.h"

#include "google/protobuf/any.pb-c.h"
#include "google/protobuf/empty.pb-c.h"
#include "mt_proto_pulse_motor.pb-c.h"
#include "stream_frame.pb-c.h"

#include "mt_module_flow_manage.h"
#include "mt_module_mqtt.h"
#include "mt_module_unarycall_utils.h"
#include "mt_mqtt_lan.h"
#include "mt_mqtt_utils.h"
#include "mt_utils_error.h"

#include "gpio_pulse_motor.h"
#include "gpio_pulse_motor_module_flow.h"
#include "gpio_pulse_motor_utils.h"

// global config ==============================================================

static const char *TAG = "MODULE_MQTT";

// static func ================================================================

static void
module_mqtt_process_get_state(Ai__Metathings__Component__DownStreamFrame *msg,
                              char *module_id) {
  esp_err_t err = ESP_OK;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  MtPulseMotor__GetStateRes res = MT_PULSE_MOTOR__GET_STATE_RES__INIT;
  res.code = MT_ERR_NO_ERR;

  // arg check
  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.code = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // cmd process
  err = gpio_pulse_motor_get_state((bool *)&res.state, &res.lefttime);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_pulse_motor_get_state failed", __LINE__,
             __func__);
    res.code = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

ERROR:
  // response
  res_size = mt_pulse_motor__get_state_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_pulse_motor__get_state_res__pack(&res, res_buf);
  err = mt_module_unarycall_utils_mqtt_sent_msg(
      res_buf, res_size, module_id,
      mt_pulse_motor__get_state_res__descriptor.name, msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == MT_ERR_NO_ERR) {
    ESP_LOGI(TAG, "%4d %s get_state success, state:%d", __LINE__, __func__,
             res.state);
  } else {
    ESP_LOGE(TAG, "%4d %s get_state failed", __LINE__, __func__);
  }

EXIT:
  if (res_buf != NULL)
    free(res_buf);
  return;
}

static void
module_mqtt_process_set_state(Ai__Metathings__Component__DownStreamFrame *msg,
                              char *module_id) {
  esp_err_t err = ESP_OK;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  MtPulseMotor__SetStateReq *req = NULL;
  MtPulseMotor__SetStateRes res = MT_PULSE_MOTOR__SET_STATE_RES__INIT;
  res.code = MT_ERR_NO_ERR;

  // arg check
  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.code = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // get req
  req = mt_pulse_motor__set_state_req__unpack(
      NULL, msg->unary_call->value->value.len,
      msg->unary_call->value->value.data);
  err = gpio_pulse_motor_utils_check_set_state_req(req);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_pulse_motor_utils_check_set_state_req failed",
             __LINE__, __func__);
    res.code = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // cmd process
  gpio_pulse_motor_clear_delay_task();
  err = gpio_pulse_motor_set_state(req->state);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_pulse_motor_set_state failed", __LINE__,
             __func__);
    res.code = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // flow notify
  gpio_pulse_motor_module_notify_state_task("net", req->state, 0);

ERROR:
  // response
  res_size = mt_pulse_motor__set_state_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_pulse_motor__set_state_res__pack(&res, res_buf);
  err = mt_module_unarycall_utils_mqtt_sent_msg(
      res_buf, res_size, module_id,
      mt_pulse_motor__set_state_res__descriptor.name, msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == MT_ERR_NO_ERR) {
    ESP_LOGI(TAG, "%4d %s set_state success", __LINE__, __func__);
  } else {
    ESP_LOGE(TAG, "%4d %s set_state failed", __LINE__, __func__);
  }

EXIT:
  if (req != NULL)
    mt_pulse_motor__set_state_req__free_unpacked(req, NULL);
  if (res_buf != NULL)
    free(res_buf);
  return;
}

static void module_mqtt_process_set_state_with_delay(
    Ai__Metathings__Component__DownStreamFrame *msg, char *module_id) {
  esp_err_t err = ESP_OK;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  MtPulseMotor__SetStateWithDelayReq *req = NULL;
  MtPulseMotor__SetStateWithDelayRes res =
      MT_PULSE_MOTOR__SET_STATE_WITH_DELAY_RES__INIT;
  res.code = MT_ERR_NO_ERR;

  // arg check
  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.code = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // get req
  req = mt_pulse_motor__set_state_with_delay_req__unpack(
      NULL, msg->unary_call->value->value.len,
      msg->unary_call->value->value.data);
  err = gpio_pulse_motor_utils_check_set_state_with_delay_req(req);
  if (err != ESP_OK) {
    ESP_LOGE(
        TAG,
        "%4d %s gpio_pulse_motor_utils_check_set_state_with_delay_req failed",
        __LINE__, __func__);
    res.code = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // cmd process
  err = gpio_pulse_motor_set_state_with_delay(req->state, req->time);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_pulse_motor_set_state_with_delay failed",
             __LINE__, __func__);
    res.code = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // flow notify
  gpio_pulse_motor_module_notify_state_task("net", req->state, req->time);

ERROR:
  // response
  res_size = mt_pulse_motor__set_state_with_delay_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_pulse_motor__set_state_with_delay_res__pack(&res, res_buf);
  err = mt_module_unarycall_utils_mqtt_sent_msg(
      res_buf, res_size, module_id,
      mt_pulse_motor__set_state_with_delay_res__descriptor.name, msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == MT_ERR_NO_ERR) {
    ESP_LOGI(TAG, "%4d %s set_state_with_delay success, state:%d time:%f",
             __LINE__, __func__, req->state, req->time);
  } else {
    ESP_LOGE(TAG, "%4d %s set_state_with_delay failed", __LINE__, __func__);
  }

EXIT:
  if (req != NULL)
    mt_pulse_motor__set_state_with_delay_req__free_unpacked(req, NULL);
  if (res_buf != NULL)
    free(res_buf);
  return;
}

// global func  ===============================================================
void gpio_pulse_motor_module_mqtt_init() {
  mt_module_mqtt_add_handle(module_mqtt_process_get_state, "GetState");
  mt_module_mqtt_add_handle(module_mqtt_process_set_state, "SetState");
  mt_module_mqtt_add_handle(module_mqtt_process_set_state_with_delay,
                            "SetStateWithDelay");
}
