#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"

#include "mt_module_unarycall_config.h"
#include "mt_module_unarycall_utils.h"
#include "mt_mqtt_lan.h"
#include "mt_sys_config.h"
#include "mt_utils_error.h"

#include "google/protobuf/any.pb-c.h"
#include "google/protobuf/empty.pb-c.h"
#include "mt_proto_sys_config.pb-c.h"
#include "stream_frame.pb-c.h"

// global define ==============================================================

static const char *TAG = "MT_MODULE_UNARYCALL_CONFIG";

// global func ================================================================

void mt_module_unarycall_config_get_int32_config_handle(
    Ai__Metathings__Component__DownStreamFrame *msg, char module_id[128]) {
  esp_err_t err = ESP_OK;
  MtSysConfig__GetIntConfigReq *req = NULL;
  MtSysConfig__GetIntConfigRes res = MT_SYS_CONFIG__GET_INT_CONFIG_RES__INIT;
  MtSysConfig__IntConfig int_config = MT_SYS_CONFIG__INT_CONFIG__INIT;
  res.intconfig = &int_config;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;

  // get req
  req = mt_sys_config__get_int_config_req__unpack(
      NULL, msg->unary_call->value->value.len,
      msg->unary_call->value->value.data);
  if (req == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_sys_config__get_int_config_req__unpack NULL",
             __LINE__, __func__);
    err = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // check req
  if (req->key == NULL) {
    ESP_LOGE(TAG, "%4d %s req->key NULL", __LINE__, __func__);
    err = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // cmd process
  err = mt_sys_config_get_int32_config(req->key, &res.intconfig->value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_sys_config_get_int32_config failed", __LINE__,
             __func__);
    err = MT_ERR_INVALID_RESPONSE;
    goto ERROR;
  }

ERROR:
  // marshall res
  res.code = err;
  res_size = mt_sys_config__get_int_config_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_sys_config__get_int_config_res__pack(&res, res_buf);
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt_sys_config__get_int_config_res__descriptor.name,
      msg, &frame_size);

  // response
  char topic[256] = "";
  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == MT_ERR_NO_ERR) {
    ESP_LOGI(TAG, "%4d %s session:%lld get int config code:%d key:%s value:%d",
             __LINE__, __func__, msg->unary_call->session->value, res.code,
             req->key, res.intconfig->value);
  } else {
    ESP_LOGE(TAG, "%4d %s session:%lld get int config code:%d", __LINE__,
             __func__, msg->unary_call->session->value, res.code);
  }

EXIT:
  if (req != NULL) {
    mt_sys_config__get_int_config_req__free_unpacked(req, NULL);
  }
  free(res_buf);
  free(frame_buf);
  return;
}

void mt_module_unarycall_config_set_int32_config_handle(
    Ai__Metathings__Component__DownStreamFrame *msg, char module_id[128]) {
  esp_err_t err = ESP_OK;
  MtSysConfig__SetIntConfigReq *req = NULL;
  MtSysConfig__SetRes res = MT_SYS_CONFIG__SET_RES__INIT;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;

  // get req
  req = mt_sys_config__set_int_config_req__unpack(
      NULL, msg->unary_call->value->value.len,
      msg->unary_call->value->value.data);
  if (req == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_sys_config__get_int_config_req__unpack NULL",
             __LINE__, __func__);
    err = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // check req
  if (req->key == NULL) {
    ESP_LOGE(TAG, "%4d %s req->key NULL", __LINE__, __func__);
    err = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // cmd process
  err = mt_sys_config_set_int32_config(req->key, req->value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_sys_config_set_int32_config failed", __LINE__,
             __func__);
    err = MT_ERR_INVALID_RESPONSE;
    goto ERROR;
  }

ERROR:
  // marshall res
  res.code = err;
  res_size = mt_sys_config__set_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_sys_config__set_res__pack(&res, res_buf);
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt_sys_config__set_res__descriptor.name, msg,
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
  if (res.code == MT_ERR_NO_ERR) {
    ESP_LOGI(TAG, "%4d %s session:%lld set int config code:%d key:%s value:%d",
             __LINE__, __func__, msg->unary_call->session->value, res.code,
             req->key, req->value);
  } else {
    ESP_LOGE(TAG, "%4d %s session:%lld set int config code:%d", __LINE__,
             __func__, msg->unary_call->session->value, res.code);
  }

EXIT:
  if (req != NULL) {
    mt_sys_config__set_int_config_req__free_unpacked(req, NULL);
  }
  free(res_buf);
  free(frame_buf);
  return;
}

void mt_module_unarycall_config_get_string_config_handle(
    Ai__Metathings__Component__DownStreamFrame *msg, char module_id[128]) {
  esp_err_t err = ESP_OK;
  MtSysConfig__GetStringConfigReq *req = NULL;
  MtSysConfig__GetStringConfigRes res =
      MT_SYS_CONFIG__GET_STRING_CONFIG_RES__INIT;
  MtSysConfig__StringConfig string_config = MT_SYS_CONFIG__STRING_CONFIG__INIT;
  res.stringconfig = &string_config;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;

  // get req
  req = mt_sys_config__get_string_config_req__unpack(
      NULL, msg->unary_call->value->value.len,
      msg->unary_call->value->value.data);
  if (req == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_sys_config__get_string_config_req__unpack NULL",
             __LINE__, __func__);
    err = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // check req
  if (req->key == NULL) {
    ESP_LOGE(TAG, "%4d %s req->key NULL", __LINE__, __func__);
    err = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // cmd process
  res.stringconfig->value = mt_sys_config_get_string_config(req->key);
  if (res.stringconfig == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_sys_config_get_string_config NULL", __LINE__,
             __func__);
    err = MT_ERR_INVALID_RESPONSE;
    goto ERROR;
  }

ERROR:
  // marshall res
  res.code = err;
  res_size = mt_sys_config__get_string_config_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_sys_config__get_string_config_res__pack(&res, res_buf);
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt_sys_config__get_string_config_res__descriptor.name,
      msg, &frame_size);

  // response
  char topic[256] = "";
  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == MT_ERR_NO_ERR) {
    ESP_LOGI(TAG,
             "%4d %s session:%lld get string config code:%d key:%s value:%s",
             __LINE__, __func__, msg->unary_call->session->value, res.code,
             req->key, res.stringconfig->value);
  } else {
    ESP_LOGE(TAG, "%4d %s session:%lld get string config code:%d", __LINE__,
             __func__, msg->unary_call->session->value, res.code);
  }

EXIT:
  if (req != NULL) {
    mt_sys_config__get_string_config_req__free_unpacked(req, NULL);
  }
  if (res.stringconfig->value != NULL) {
    free(res.stringconfig->value);
  }
  free(res_buf);
  free(frame_buf);
  return;
}

void mt_module_unarycall_config_set_string_config_handle(
    Ai__Metathings__Component__DownStreamFrame *msg, char module_id[128]) {
  esp_err_t err = ESP_OK;
  MtSysConfig__SetStringConfigReq *req = NULL;
  MtSysConfig__SetRes res = MT_SYS_CONFIG__SET_RES__INIT;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;

  // get req
  req = mt_sys_config__set_string_config_req__unpack(
      NULL, msg->unary_call->value->value.len,
      msg->unary_call->value->value.data);
  if (req == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_sys_config__get_int_config_req__unpack NULL",
             __LINE__, __func__);
    err = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // check req
  if (req->key == NULL) {
    ESP_LOGE(TAG, "%4d %s req->key NULL", __LINE__, __func__);
    err = MT_ERR_INVALID_ARG;
    goto ERROR;
  }
  if (req->value == NULL) {
    ESP_LOGE(TAG, "%4d %s req->value NULL", __LINE__, __func__);
    err = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // cmd process
  err = mt_sys_config_set_string_config(req->key, req->value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_sys_config_set_string_config failed", __LINE__,
             __func__);
    err = MT_ERR_INVALID_RESPONSE;
    goto ERROR;
  }

ERROR:
  // marshall res
  res.code = err;
  res_size = mt_sys_config__set_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_sys_config__set_res__pack(&res, res_buf);
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt_sys_config__set_res__descriptor.name, msg,
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

  if (res.code == MT_ERR_NO_ERR) {
    ESP_LOGI(TAG,
             "%4d %s session:%lld set string config code:%d key:%s value:%s",
             __LINE__, __func__, msg->unary_call->session->value, res.code,
             req->key, req->value);
  } else {
    ESP_LOGE(TAG, "%4d %s session:%lld get host config code:%d", __LINE__,
             __func__, msg->unary_call->session->value, res.code);
  }

EXIT:
  if (req != NULL) {
    mt_sys_config__set_string_config_req__free_unpacked(req, NULL);
  }
  free(res_buf);
  free(frame_buf);
  return;
}

void mt_module_unarycall_config_get_host_config_handle(
    Ai__Metathings__Component__DownStreamFrame *msg, char module_id[128]) {
  esp_err_t err = ESP_OK;
  MtSysConfig__GetHostConfigRes res = MT_SYS_CONFIG__GET_HOST_CONFIG_RES__INIT;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;
  mt_sys_config_host *temp_host = NULL;

  // cmd process
  temp_host = mt_sys_config_get_host_config();
  if (temp_host == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_sys_config_get_host_config NULL", __LINE__,
             __func__);
    err = MT_ERR_INVALID_RESPONSE;
    goto ERROR;
  }
  res.hostconfig = malloc(sizeof(MtSysConfig__HostConfig));
  res.hostconfig->host = temp_host->host;
  res.hostconfig->usessl = temp_host->use_ssl;
  res.hostconfig->httpport = temp_host->http_port;
  res.hostconfig->mqtthost = temp_host->mqtt_host;
  res.hostconfig->mqttport = temp_host->mqtt_port;
  res.hostconfig->credid = temp_host->module_cred_id;
  res.hostconfig->credkey = temp_host->module_cred_key;

ERROR:
  // marshall res
  res.code = err;
  res_size = mt_sys_config__get_host_config_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_sys_config__get_host_config_res__pack(&res, res_buf);
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt_sys_config__get_host_config_res__descriptor.name,
      msg, &frame_size);

  // response
  char topic[256] = "";
  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == MT_ERR_NO_ERR) {
    ESP_LOGI(TAG,
             "%4d %s session:%lld get host config code:%d host:%s usessl:%d "
             "http_port:%d mqtt_host:%s mqtt_port:%d cred_id:%s",
             __LINE__, __func__, msg->unary_call->session->value, res.code,
             res.hostconfig->host, res.hostconfig->usessl,
             res.hostconfig->httpport, res.hostconfig->mqtthost,
             res.hostconfig->mqttport, res.hostconfig->credid);
  } else {
    ESP_LOGE(TAG, "%4d %s session:%lld get host config code:%d", __LINE__,
             __func__, msg->unary_call->session->value, res.code);
  }

EXIT:
  if (res.hostconfig != NULL) {
    mt_sys_config__host_config__free_unpacked(res.hostconfig, NULL);
  }
  if (temp_host != NULL) {
    free(temp_host);
  }
  free(res_buf);
  free(frame_buf);
  return;
}

void mt_module_unarycall_config_set_host_config_handle(
    Ai__Metathings__Component__DownStreamFrame *msg, char module_id[128]) {
  esp_err_t err = ESP_OK;
  MtSysConfig__SetHostConfigReq *req = NULL;
  MtSysConfig__SetRes res = MT_SYS_CONFIG__SET_RES__INIT;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;
  mt_sys_config_host *temp_host = malloc(sizeof(mt_sys_config_host));

  // get req
  req = mt_sys_config__set_host_config_req__unpack(
      NULL, msg->unary_call->value->value.len,
      msg->unary_call->value->value.data);
  if (req == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_sys_config__set_host_config_req__unpack NULL",
             __LINE__, __func__);
    err = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // check req
  if (req->hostconfig == NULL) {
    ESP_LOGE(TAG, "%4d %s req->hostconfig NULL", __LINE__, __func__);
    err = MT_ERR_INVALID_ARG;
    goto ERROR;
  } else {
    if (req->hostconfig->host == NULL) {
      ESP_LOGE(TAG, "%4d %s req->hostconfig->host NULL", __LINE__, __func__);
      err = MT_ERR_INVALID_ARG;
      goto ERROR;
    }
    if (req->hostconfig->mqtthost == NULL) {
      ESP_LOGE(TAG, "%4d %s req->hostconfig->mqtthost NULL", __LINE__,
               __func__);
      err = MT_ERR_INVALID_ARG;
      goto ERROR;
    }
    if (req->hostconfig->credid == NULL) {
      ESP_LOGE(TAG, "%4d %s req->hostconfig->credid NULL", __LINE__, __func__);
      err = MT_ERR_INVALID_ARG;
      goto ERROR;
    }
    if (req->hostconfig->credkey == NULL) {
      ESP_LOGE(TAG, "%4d %s req->hostconfig->credkey NULL", __LINE__, __func__);
      err = MT_ERR_INVALID_ARG;
      goto ERROR;
    }
  }
  temp_host->host = req->hostconfig->host;
  temp_host->use_ssl = req->hostconfig->usessl;
  temp_host->http_port = req->hostconfig->httpport;
  temp_host->mqtt_host = req->hostconfig->mqtthost;
  temp_host->mqtt_port = req->hostconfig->mqttport;
  temp_host->module_cred_id = req->hostconfig->credid;
  temp_host->module_cred_key = req->hostconfig->credkey;

  // cmd process
  err = mt_sys_config_set_host_config(temp_host);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_sys_config_set_host_config failed", __LINE__,
             __func__);
    res.code = MT_ERR_INVALID_RESPONSE;
    goto ERROR;
  }

ERROR:
  // marshall res
  res.code = err;
  res_size = mt_sys_config__set_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_sys_config__set_res__pack(&res, res_buf);
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt_sys_config__set_res__descriptor.name, msg,
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

  if (res.code == MT_ERR_NO_ERR) {
    ESP_LOGI(TAG,
             "%4d %s session:%lld set host config code:%d host:%s usessl:%d "
             "http_port:%d mqtt_port:%d cred_id:%s",
             __LINE__, __func__, msg->unary_call->session->value, res.code,
             req->hostconfig->host, req->hostconfig->usessl,
             req->hostconfig->httpport, req->hostconfig->mqttport,
             req->hostconfig->credid);
  } else {
    ESP_LOGE(TAG, "%4d %s session:%lld set host config code:%d", __LINE__,
             __func__, msg->unary_call->session->value, res.code);
  }

EXIT:
  if (req != NULL) {
    mt_sys_config__set_host_config_req__free_unpacked(req, NULL);
  }
  free(temp_host);
  free(res_buf);
  free(frame_buf);
  return;
}
