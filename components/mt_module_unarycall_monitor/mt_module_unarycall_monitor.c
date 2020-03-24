#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"

#include "mt_module_unarycall_monitor.h"
#include "mt_module_unarycall_utils.h"
#include "mt_mqtt_lan.h"
#include "mt_sys_monitor.h"
#include "mt_utils_error.h"

#include "google/protobuf/any.pb-c.h"
#include "google/protobuf/empty.pb-c.h"
#include "mt_proto_sys_monitor.pb-c.h"
#include "stream_frame.pb-c.h"

// global define ==============================================================

static const char *TAG = "MT_MODULE_UNARYCALL_MONITOR";

// global func ================================================================

void mt_module_unarycall_monitor_get_state_handle(
    Ai__Metathings__Component__DownStreamFrame *msg, char module_id[128]) {
  esp_err_t err = ESP_OK;
  MtSysMonitor__GetStateRes res = MT_SYS_MONITOR__GET_STATE_RES__INIT;
  res.code = MT_ERR_NO_ERR;
  MtSysMonitor__State state = MT_SYS_MONITOR__STATE__INIT;
  res.state = &state;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;
  mt_sys_monitor_state *temp_state = NULL;

  // cmd process
  temp_state = mt_sys_monitor_get_state();
  if (temp_state == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_sys_monitor_get_state NULL", __LINE__, __func__);
    err = MT_ERR_INVALID_RESPONSE;
    goto ERROR;
  }
  res.state->startup = temp_state->startup;
  res.state->restartcount = temp_state->restart_count;
  res.state->free = temp_state->free;
  res.state->errorcount = temp_state->error_count;

ERROR:
  // marshall res
  res.code = err;
  res_size = mt_sys_monitor__get_state_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_sys_monitor__get_state_res__pack(&res, res_buf);
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt_sys_monitor__get_state_res__descriptor.name, msg,
      &frame_size);

  // response
  char topic[256] = "";
  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code != MT_ERR_NO_ERR) {
    ESP_LOGI(
        TAG,
        "%4d %s session:%lld get state code:%d startup:%d restart_count:%d "
        "free:%.2f error_count:%d",
        __LINE__, __func__, msg->unary_call->session->value, res.code,
        res.state->startup, res.state->restartcount, res.state->free,
        res.state->errorcount);
  } else {
    ESP_LOGE(TAG, "%4d %s session:%lld get state code:%d", __LINE__, __func__,
             msg->unary_call->session->value, res.code);
  }

EXIT:
  if (temp_state != NULL) {
    free(temp_state);
  }
  free(res_buf);
  free(frame_buf);
  return;
}

void mt_module_unarycall_monitor_set_restart_handle(
    Ai__Metathings__Component__DownStreamFrame *msg, char module_id[128]) {
  esp_err_t err = ESP_OK;
  MtSysMonitor__SetRestartRes res = MT_SYS_MONITOR__SET_RESTART_RES__INIT;
  res.code = MT_ERR_NO_ERR;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;

  // cmd process
  err = mt_sys_monitor_set_restart();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_sys_monitor_set_restart failed", __LINE__,
             __func__);
    err = MT_ERR_INVALID_RESPONSE;
    goto ERROR;
  }

ERROR:
  // marshall res
  res.code = err;
  res_size = mt_sys_monitor__set_restart_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_sys_monitor__set_restart_res__pack(&res, res_buf);
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt_sys_monitor__set_restart_res__descriptor.name, msg,
      &frame_size);

  // response
  char topic[256] = "";
  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code != MT_ERR_NO_ERR) {
    ESP_LOGI(TAG, "%4d %s session:%lld set restart code:%d ", __LINE__,
             __func__, msg->unary_call->session->value, res.code);
  } else {
    ESP_LOGE(TAG, "%4d %s session:%lld set restart code:%d", __LINE__, __func__,
             msg->unary_call->session->value, res.code);
  }

EXIT:
  free(res_buf);
  free(frame_buf);
  return;
}
