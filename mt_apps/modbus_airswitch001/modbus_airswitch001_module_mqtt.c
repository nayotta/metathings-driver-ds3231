#include "stdio.h"
#include "string.h"

#include "esp_err.h"
#include "esp_log.h"
#include "mqtt_client.h"

#include "google/protobuf/any.pb-c.h"
#include "google/protobuf/empty.pb-c.h"
#include "stream_frame.pb-c.h"

#include "cache_airswitch001.h"
#include "modbus_airswitch001.h"
#include "modbus_airswitch001_module_flow.h"
#include "modbus_airswitch001_notify.h"
#include "modbus_airswitch001_utils.h"
#include "mt_module_flow.h"
#include "mt_module_flow_manage.h"
#include "mt_module_mqtt.h"
#include "mt_module_unarycall_utils.h"
#include "mt_mqtt_lan.h"
#include "mt_mqtt_utils.h"
#include "mt_proto_airswitch001.pb-c.h"

// global config ==============================================================

static const char *TAG = "MODULE_MQTT";

#define MOD_ADDR 1
static const int MAX_KEY_SIZE = 20;

// static func ================================================================
static void
module_mqtt_process_get_state(Ai__Metathings__Component__DownStreamFrame *msg,
                              char *module_id) {
  esp_err_t err = ESP_OK;
  MtAirswitch001__GetReq *req = NULL;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;
  MtAirswitch001__GetStateRes res = MT_AIRSWITCH001__GET_STATE_RES__INIT;
  MtAirswitch001__State state = MT_AIRSWITCH001__STATE__INIT;
  res.state = &state;
  int32_t switch_addr = -1;
  char topic[256] = "";

  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  }

  req =
      mt_airswitch001__get_req__unpack(NULL, msg->unary_call->value->value.len,
                                       msg->unary_call->value->value.data);
  err = modbus_airswitch001_util_check_get_req(&switch_addr, req);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_airswitch001_util_check_get_req failed",
             __LINE__, __func__);
    res.code = 2;
    goto ERROR;
  }

  // cmd process
  res.addr = switch_addr;
  err = mt_airswitch001_get_state(MOD_ADDR, switch_addr,
                                  (bool *)&(res.state->data));
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_airswitch001_get_state failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  }

ERROR:
  res_size = mt_airswitch001__get_state_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_airswitch001__get_state_res__pack(&res, res_buf);

  // marsharll data
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt_airswitch001__get_state_res__descriptor.name, msg,
      &frame_size);

  // response
  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == 0)
    ESP_LOGI(TAG, "%4d %s session:%lld addr:%d get state:%d response", __LINE__,
             __func__, msg->unary_call->session->value, switch_addr,
             res.state->data);
  else
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);

EXIT:
  if (req != NULL)
    mt_airswitch001__get_req__free_unpacked(req, NULL);
  free(res_buf);
  free(frame_buf);
  return;
}

static void
module_mqtt_process_get_ctrl(Ai__Metathings__Component__DownStreamFrame *msg,
                             char *module_id) {
  esp_err_t err = ESP_OK;
  MtAirswitch001__GetReq *req = NULL;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;
  MtAirswitch001__GetCtrlRes res = MT_AIRSWITCH001__GET_CTRL_RES__INIT;
  MtAirswitch001__Ctrl ctrl = MT_AIRSWITCH001__CTRL__INIT;
  res.ctrl = &ctrl;
  int32_t switch_addr = -1;
  char topic[256] = "";

  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  }

  req =
      mt_airswitch001__get_req__unpack(NULL, msg->unary_call->value->value.len,
                                       msg->unary_call->value->value.data);
  err = modbus_airswitch001_util_check_get_req(&switch_addr, req);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_airswitch001_util_check_get_req failed",
             __LINE__, __func__);
    res.code = 2;
    goto ERROR;
  }

  // cmd process
  res.addr = switch_addr;
  err =
      mt_airswitch001_get_ctrl(MOD_ADDR, switch_addr, (bool *)&res.ctrl->data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_airswitch001_get_ctrl failed", __LINE__, __func__);
    res.code = 2;
    goto ERROR;
  }

ERROR:
  res_size = mt_airswitch001__get_ctrl_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_airswitch001__get_ctrl_res__pack(&res, res_buf);

  // marsharll data
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt_airswitch001__get_ctrl_res__descriptor.name, msg,
      &frame_size);

  // response
  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == 0)
    ESP_LOGI(TAG, "%4d %s session:%lld addr:%d get ctrl:%d response", __LINE__,
             __func__, msg->unary_call->session->value, switch_addr,
             res.ctrl->data);
  else
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);

EXIT:
  if (req != NULL)
    mt_airswitch001__get_req__free_unpacked(req, NULL);
  free(res_buf);
  free(frame_buf);
  return;
}

static void
module_mqtt_process_get_warn(Ai__Metathings__Component__DownStreamFrame *msg,
                             char *module_id) {
  esp_err_t err = ESP_OK;
  MtAirswitch001__GetReq *req = NULL;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;
  MtAirswitch001__GetWarnRes res = MT_AIRSWITCH001__GET_WARN_RES__INIT;
  MtAirswitch001__Warn warn = MT_AIRSWITCH001__WARN__INIT;
  res.warn = &warn;
  airswitch_warn_t switch_warn;
  int32_t switch_addr = -1;
  char topic[256] = "";

  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  }

  req =
      mt_airswitch001__get_req__unpack(NULL, msg->unary_call->value->value.len,
                                       msg->unary_call->value->value.data);
  err = modbus_airswitch001_util_check_get_req(&switch_addr, req);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_airswitch001_util_check_get_req failed",
             __LINE__, __func__);
    res.code = 2;
    goto ERROR;
  }

  // cmd process
  res.addr = switch_addr;
  err = mt_airswitch001_get_warn(MOD_ADDR, switch_addr, &switch_warn);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_airswitch001_get_warn failed", __LINE__, __func__);
    res.code = 2;
    goto ERROR;
  }

  res.warn->notecurrent = switch_warn.note_current;
  res.warn->noteleak = switch_warn.note_leak;
  res.warn->notevotagelow = switch_warn.note_votage_low;
  res.warn->notevotagehigh = switch_warn.note_votage_high;
  res.warn->warnfire = switch_warn.warn_fire;
  res.warn->warncurrent = switch_warn.warn_current;
  res.warn->warnleak = switch_warn.warn_leak;
  res.warn->warnpowrhigh = switch_warn.warn_power;
  res.warn->warnshort = switch_warn.warn_short;
  res.warn->warntemphigh = switch_warn.warn_temp;
  res.warn->warnvotagehigh = switch_warn.warn_votage_high;
  res.warn->warnvotagelow = switch_warn.warn_votage_low;
  res.warn->warnwave = switch_warn.warn_wave;

ERROR:
  res_size = mt_airswitch001__get_warn_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_airswitch001__get_warn_res__pack(&res, res_buf);

  // marsharll data
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt_airswitch001__get_warn_res__descriptor.name, msg,
      &frame_size);

  // response
  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == 0)
    ESP_LOGI(
        TAG,
        "%4d %s session:%lld addr:%d get warn,noteleak:%d notevotagehigh:%d "
        "notevotagelow:%d warncurrent:%d warnfire:%d "
        "warnleak:%d warnpowerhigh:%d warnshort:%d "
        "warntemphigh:%d warnvotagehigh:%d "
        "warnvotagelow:%d warnwave:%d "
        "response",
        __LINE__, __func__, msg->unary_call->session->value, switch_addr,
        res.warn->noteleak, res.warn->notevotagehigh, res.warn->notevotagelow,
        res.warn->warncurrent, res.warn->warnfire, res.warn->warnleak,
        res.warn->warnpowrhigh, res.warn->warnshort, res.warn->warntemphigh,
        res.warn->warnvotagehigh, res.warn->warnvotagelow, res.warn->warnwave);
  else
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);

EXIT:
  if (req != NULL)
    mt_airswitch001__get_req__free_unpacked(req, NULL);
  free(res_buf);
  free(frame_buf);
  return;
}

static void
module_mqtt_process_get_data(Ai__Metathings__Component__DownStreamFrame *msg,
                             char *module_id) {
  esp_err_t err = ESP_OK;
  MtAirswitch001__GetReq *req = NULL;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;
  MtAirswitch001__GetDataRes res = MT_AIRSWITCH001__GET_DATA_RES__INIT;
  MtAirswitch001__Data data = MT_AIRSWITCH001__DATA__INIT;
  res.data = &data;
  airswitch_data_t datas;
  int32_t switch_addr = -1;
  char topic[256] = "";

  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  }

  req =
      mt_airswitch001__get_req__unpack(NULL, msg->unary_call->value->value.len,
                                       msg->unary_call->value->value.data);
  err = modbus_airswitch001_util_check_get_req(&switch_addr, req);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_airswitch001_util_check_get_req failed",
             __LINE__, __func__);
    res.code = 2;
    goto ERROR;
  }

  // cmd process
  res.addr = switch_addr;
  err = mt_airswitch001_get_datas(MOD_ADDR, switch_addr, &datas.state,
                                  &datas.ctrl, &datas.votage,
                                  &datas.leak_current, &datas.power,
                                  &datas.temp, &datas.current, &datas.quality);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_airswitch001_get_datas failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  }

  err = modbus_airswitch001_util_copy_datas(&datas, res.data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_airswitch001_util_copy_datas failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  }

ERROR:
  res_size = mt_airswitch001__get_data_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_airswitch001__get_data_res__pack(&res, res_buf);

  // marsharll data
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt_airswitch001__get_data_res__descriptor.name, msg,
      &frame_size);

  // response
  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == 0)
    ESP_LOGI(TAG,
             "%4d %s session:%lld addr:%d get datas, "
             "ctrl:%d current:%f leakcurrent:%f "
             "power:%f state:%d temp:%f votage:%f"
             "response ",
             __LINE__, __func__, msg->unary_call->session->value, switch_addr,
             res.data->ctrl, res.data->current, res.data->leakcurrent,
             res.data->power, res.data->state, res.data->temp,
             res.data->votage);
  else
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);

EXIT:
  if (req != NULL)
    mt_airswitch001__get_req__free_unpacked(req, NULL);
  free(res_buf);
  free(frame_buf);
  return;
}

static void
module_mqtt_process_get_config(Ai__Metathings__Component__DownStreamFrame *msg,
                               char *module_id) {
  esp_err_t err = ESP_OK;
  MtAirswitch001__GetReq *req = NULL;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;
  MtAirswitch001__GetConfigRes res = MT_AIRSWITCH001__GET_CONFIG_RES__INIT;
  MtAirswitch001__Config config = MT_AIRSWITCH001__CONFIG__INIT;
  res.config = &config;
  airswitch_config_t configs;
  int32_t switch_addr = -1;
  char topic[256] = "";

  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  }

  req =
      mt_airswitch001__get_req__unpack(NULL, msg->unary_call->value->value.len,
                                       msg->unary_call->value->value.data);
  err = modbus_airswitch001_util_check_get_req(&switch_addr, req);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_airswitch001_util_check_get_req failed",
             __LINE__, __func__);
    res.code = 2;
    goto ERROR;
  }

  // cmd process
  res.addr = switch_addr;
  err = mt_airswitch001_get_configs(
      MOD_ADDR, switch_addr, &configs.votage_high, &configs.votage_low,
      &configs.leak_current_high, &configs.power_high, &configs.temp_high,
      &configs.current_high);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_airswitch001_get_configs failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  }

  err = modbus_airswitch001_util_copy_configs(&configs, res.config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_airswitch001_util_copy_configs failed",
             __LINE__, __func__);
    res.code = 2;
    goto ERROR;
  }

ERROR:
  res_size = mt_airswitch001__get_config_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_airswitch001__get_config_res__pack(&res, res_buf);

  // marsharll data
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt_airswitch001__get_config_res__descriptor.name, msg,
      &frame_size);

  // response
  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == 0)
    ESP_LOGI(TAG,
             "%4d %s session:%lld addr:%d get config, "
             "votage_high:%f  votage_low:%f "
             "leak_current:%f power_high:%f "
             "temp_high:%f current_high:%f "
             "response ",
             __LINE__, __func__, msg->unary_call->session->value, switch_addr,
             res.config->votagehigh, res.config->votagelow,
             res.config->leakcurrenthigh, res.config->powerhigh,
             res.config->temphigh, res.config->currenthigh);
  else
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);

EXIT:
  if (req != NULL)
    mt_airswitch001__get_req__free_unpacked(req, NULL);
  free(res_buf);
  free(frame_buf);
  return;
}

static void
module_mqtt_process_get_quality(Ai__Metathings__Component__DownStreamFrame *msg,
                                char *module_id) {
  esp_err_t err = ESP_OK;
  MtAirswitch001__GetReq *req = NULL;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;
  MtAirswitch001__GetQualityRes res = MT_AIRSWITCH001__GET_QUALITY_RES__INIT;
  MtAirswitch001__Quality quality = MT_AIRSWITCH001__QUALITY__INIT;
  res.quality = &quality;
  int32_t switch_addr = -1;
  double quality_out = 0;
  char topic[256] = "";

  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  }

  req =
      mt_airswitch001__get_req__unpack(NULL, msg->unary_call->value->value.len,
                                       msg->unary_call->value->value.data);
  err = modbus_airswitch001_util_check_get_req(&switch_addr, req);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_airswitch001_util_check_get_req failed",
             __LINE__, __func__);
    res.code = 2;
    goto ERROR;
  }

  // cmd process
  res.addr = switch_addr;
  err = mt_airswitch001_get_cache_quality(MOD_ADDR, switch_addr, &quality_out);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_airswitch001_get_state failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  }
  res.quality->quality = quality_out;

ERROR:
  res_size = mt_airswitch001__get_quality_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_airswitch001__get_quality_res__pack(&res, res_buf);

  // marsharll data
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt_airswitch001__get_quality_res__descriptor.name, msg,
      &frame_size);

  // response
  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == 0)
    ESP_LOGI(TAG, "%4d %s session:%lld addr:%d get quality:%f response",
             __LINE__, __func__, msg->unary_call->session->value, switch_addr,
             res.quality->quality);
  else
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);

EXIT:
  if (req != NULL)
    mt_airswitch001__get_req__free_unpacked(req, NULL);
  free(res_buf);
  free(frame_buf);
  return;
}

static void
module_mqtt_process_set_state(Ai__Metathings__Component__DownStreamFrame *msg,
                              char *module_id) {
  esp_err_t err = ESP_OK;
  MtAirswitch001__SetStateReq *req = NULL;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;
  MtAirswitch001__SetRes res = MT_AIRSWITCH001__SET_RES__INIT;
  int32_t switch_addr = -1;
  char topic[256] = "";

  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  }

  // parse req
  req = mt_airswitch001__set_state_req__unpack(
      NULL, msg->unary_call->value->value.len,
      msg->unary_call->value->value.data);
  err = modbus_airswitch001_util_check_set_state_req(&switch_addr, req);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_airswitch001_util_check_set_state_req failed",
             __LINE__, __func__);
    res.code = 2;
    goto ERROR;
  }

  // cmd process
  res.addr = switch_addr;
  bool ctrl = false;
  err = mt_airswitch001_get_ctrl(MOD_ADDR, switch_addr, &ctrl);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_airswitch001_get_ctrl failed", __LINE__, __func__);
    res.code = 2;
    goto ERROR;
  } else {
    if (ctrl == true) {
      err = mt_airswitch001_set_state(MOD_ADDR, switch_addr,
                                      req->state->data->value);
      if (err != ESP_OK) {
        ESP_LOGE(TAG, "%4d %s mt_airswitch001_set_state failed", __LINE__,
                 __func__);
        res.code = 2;
        goto ERROR;
      } else {
        res.code = 0;
      }
    } else {
      ESP_LOGE(TAG, "%4d %s addr:%d could not ctrl", __LINE__, __func__,
               switch_addr);
      res.code = 1;
      goto ERROR;
    }
  }

ERROR:
  res_size = mt_airswitch001__set_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_airswitch001__set_res__pack(&res, res_buf);

  // marshal data
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt_airswitch001__set_res__descriptor.name, msg,
      &frame_size);

  // response
  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == 0) {
    ESP_LOGI(TAG, "%4d %s session:%lld addr:%d set state:%d response", __LINE__,
             __func__, msg->unary_call->session->value, switch_addr,
             req->state->data->value);

    // notify
    mt_module_flow_struct_group_t *group =
        mt_module_flow_new_struct_group_with_notify(2);
    group->value[0]->key = malloc(MAX_KEY_SIZE);
    sprintf(group->value[0]->key, "state%d", switch_addr);
    group->value[0]->type = GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE;
    group->value[0]->bool_value = req->state->data->value;

    group->value[1]->key = malloc(MAX_KEY_SIZE);
    sprintf(group->value[1]->key, "from");
    group->value[1]->type = GOOGLE__PROTOBUF__VALUE__KIND_STRING_VALUE;
    group->value[1]->string_value = malloc(strlen("net") + 1);
    sprintf(group->value[1]->string_value, "net");
    if (module_notify_process(group) != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s module_notify_process failed", __LINE__, __func__);
    }
    mt_module_flow_free_struct_group(group);

    modbus_airswitch001_notify_update_switch(switch_addr,
                                             req->state->data->value);
  }

  else
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);

EXIT:
  if (req != NULL)
    mt_airswitch001__set_state_req__free_unpacked(req, NULL);

  free(res_buf);
  free(frame_buf);
  return;
}

static void
module_mqtt_process_set_config(Ai__Metathings__Component__DownStreamFrame *msg,
                               char *module_id) {
  esp_err_t err = ESP_OK;
  MtAirswitch001__SetConfigReq *req = NULL;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;
  MtAirswitch001__SetRes res = MT_AIRSWITCH001__SET_RES__INIT;
  double *votage_high = NULL;
  double *votage_low = NULL;
  double *leak_current = NULL;
  double *power_high = NULL;
  double *temp_high = NULL;
  double *current_high = NULL;
  int32_t switch_addr = -1;
  char topic[256] = "";

  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  }

  req = mt_airswitch001__set_config_req__unpack(
      NULL, msg->unary_call->value->value.len,
      msg->unary_call->value->value.data);
  err = modbus_airswitch001_util_check_set_config_req(&switch_addr, req);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_airswitch001_util_check_set_config_req failed",
             __LINE__, __func__);
    res.code = 2;
    goto ERROR;
  }

  if (req->config->votagehigh != NULL) {
    votage_high = malloc(sizeof(double));
    *votage_high = req->config->votagehigh->value;
  }

  if (req->config->votagelow != NULL) {
    votage_low = malloc(sizeof(double));
    *votage_low = req->config->votagelow->value;
  }

  if (req->config->leakcurrenthigh != NULL) {
    leak_current = malloc(sizeof(double));
    *leak_current = req->config->leakcurrenthigh->value;
  }

  if (req->config->powerhigh != NULL) {
    power_high = malloc(sizeof(double));
    *power_high = req->config->powerhigh->value;
  }

  if (req->config->temphigh != NULL) {
    temp_high = malloc(sizeof(double));
    *temp_high = req->config->temphigh->value;
  }

  if (req->config->currenthigh != NULL) {
    current_high = malloc(sizeof(double));
    *current_high = req->config->currenthigh->value;
  }

  // cmd process
  err = mt_airswitch001_set_configs(MOD_ADDR, switch_addr, votage_high,
                                    votage_low, leak_current, power_high,
                                    temp_high, current_high);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_airswitch001_set_configs failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  } else {
    res.code = 0;
  }

ERROR:
  res_size = mt_airswitch001__set_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_airswitch001__set_res__pack(&res, res_buf);

  // marshal data
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt_airswitch001__set_res__descriptor.name, msg,
      &frame_size);

  // response
  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == 0) {
    ESP_LOGI(TAG, "%4d %s session:%lld addr:%d set configs response", __LINE__,
             __func__, msg->unary_call->session->value, switch_addr);

    if (votage_high != NULL)
      ESP_LOGI(TAG, "%4d %s set votage high:%f", __LINE__, __func__,
               *votage_high);

    if (votage_low != NULL)
      ESP_LOGI(TAG, "%4d %s set votage low:%f", __LINE__, __func__,
               *votage_low);

    if (leak_current != NULL)
      ESP_LOGI(TAG, "%4d %s set leak_current high:%f", __LINE__, __func__,
               *leak_current);

    if (power_high != NULL)
      ESP_LOGI(TAG, "%4d %s set power high:%f", __LINE__, __func__,
               *power_high);

    if (temp_high != NULL)
      ESP_LOGI(TAG, "%4d %s set temp high:%f", __LINE__, __func__, *temp_high);

    if (current_high != NULL)
      ESP_LOGI(TAG, "%4d %s set current high:%f", __LINE__, __func__,
               *current_high);
  } else
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);

EXIT:
  if (req != NULL) {
    mt_airswitch001__set_config_req__free_unpacked(req, NULL);
  }
  free(res_buf);
  free(frame_buf);
  if (votage_high != NULL)
    free(votage_high);
  if (votage_low != NULL)
    free(votage_low);
  if (leak_current != NULL)
    free(leak_current);
  if (power_high != NULL)
    free(power_high);
  if (temp_high != NULL)
    free(temp_high);
  if (current_high != NULL)
    free(current_high);

  return;
}

static void
module_mqtt_process_set_quality(Ai__Metathings__Component__DownStreamFrame *msg,
                                char *module_id) {
  esp_err_t err = ESP_OK;
  MtAirswitch001__SetQualityReq *req = NULL;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;
  MtAirswitch001__SetRes res = MT_AIRSWITCH001__SET_RES__INIT;
  int32_t switch_addr = -1;
  char topic[256] = "";

  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  }

  req = mt_airswitch001__set_quality_req__unpack(
      NULL, msg->unary_call->value->value.len,
      msg->unary_call->value->value.data);
  err = modbus_airswitch001_util_check_set_quality_req(&switch_addr, req);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_airswitch001_util_check_get_req failed",
             __LINE__, __func__);
    res.code = 2;
    goto ERROR;
  }

  // cmd process
  res.addr = switch_addr;
  double quality_in = req->quality->quality->value;
  err = mt_airswitch001_set_cache_quality(MOD_ADDR, switch_addr, quality_in);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_airswitch001_set_cache_quality failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  }
  res.code = 0;

ERROR:
  res_size = mt_airswitch001__set_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_airswitch001__set_res__pack(&res, res_buf);

  // marshal data
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt_airswitch001__set_res__descriptor.name, msg,
      &frame_size);

  // response
  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == 0)
    ESP_LOGI(TAG, "%4d %s session:%lld addr:%d set quality:%f response",
             __LINE__, __func__, msg->unary_call->session->value, switch_addr,
             req->quality->quality->value);
  else
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);

EXIT:
  if (req != NULL)
    mt_airswitch001__set_quality_req__free_unpacked(req, NULL);
  free(res_buf);
  free(frame_buf);
  return;
}

static void module_mqtt_process_set_leak_test(
    Ai__Metathings__Component__DownStreamFrame *msg, char *module_id) {
  esp_err_t err = ESP_OK;
  MtAirswitch001__SetLeakTestReq *req = NULL;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;
  MtAirswitch001__SetRes res = MT_AIRSWITCH001__SET_RES__INIT;
  int32_t switch_addr = -1;
  char topic[256] = "";

  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  }

  req = mt_airswitch001__set_leak_test_req__unpack(
      NULL, msg->unary_call->value->value.len,
      msg->unary_call->value->value.data);
  err = modbus_airswitch001_util_check_set_leak_test_req(&switch_addr, req);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_airswitch001_util_check_get_req failed",
             __LINE__, __func__);
    res.code = 2;
    goto ERROR;
  }

  // cmd process
  res.addr = switch_addr;
  err = mt_airswitch001_set_leak_test(MOD_ADDR, switch_addr);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_airswitch001_set_leak_test failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  }
  res.code = 0;

ERROR:
  res_size = mt_airswitch001__set_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_airswitch001__set_res__pack(&res, res_buf);

  // marshal data
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt_airswitch001__set_res__descriptor.name, msg,
      &frame_size);

  // response
  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == 0)
    ESP_LOGI(TAG, "%4d %s session:%lld addr:%d set leak test response",
             __LINE__, __func__, msg->unary_call->session->value, switch_addr);
  else
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);

EXIT:
  if (req != NULL)
    mt_airswitch001__set_leak_test_req__free_unpacked(req, NULL);
  free(res_buf);
  free(frame_buf);
  return;
}

// global func  ===============================================================
void module_mqtt_init() {
  mt_module_mqtt_add_handle(module_mqtt_process_get_state, "GetState");
  mt_module_mqtt_add_handle(module_mqtt_process_get_ctrl, "GetCtrl");
  mt_module_mqtt_add_handle(module_mqtt_process_get_warn, "GetWarn");
  mt_module_mqtt_add_handle(module_mqtt_process_get_data, "GetData");
  mt_module_mqtt_add_handle(module_mqtt_process_get_config, "GetConfig");
  mt_module_mqtt_add_handle(module_mqtt_process_get_quality, "GetQuality");
  mt_module_mqtt_add_handle(module_mqtt_process_set_state, "SetState");
  mt_module_mqtt_add_handle(module_mqtt_process_set_config, "SetConfig");
  mt_module_mqtt_add_handle(module_mqtt_process_set_quality, "SetQuality");
  mt_module_mqtt_add_handle(module_mqtt_process_set_leak_test, "SetLeakTest");
}
