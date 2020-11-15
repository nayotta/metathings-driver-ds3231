#include "stdio.h"
#include "string.h"

#include "esp_err.h"
#include "esp_log.h"

#include "google/protobuf/any.pb-c.h"
#include "google/protobuf/empty.pb-c.h"
#include "stream_frame.pb-c.h"

#include "mt_module_flow_manage.h"
#include "mt_module_mqtt.h"
#include "mt_module_unarycall_utils.h"
#include "mt_mqtt_lan.h"
#include "mt_mqtt_utils.h"
#include "mt_proto_EA900II.pb-c.h"

#include "rs232_EA900II.h"

// global config ==============================================================

static const char *TAG = "MODULE_MQTT";

// static func ================================================================

static void
module_mqtt_process_get_status(Ai__Metathings__Component__DownStreamFrame *msg,
                               char *module_id) {
  esp_err_t err = ESP_OK;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;
  MtEA900II__DataRes res = MT__EA900_II__DATA_RES__INIT;
  MtEA900II__Data data = MT__EA900_II__DATA__INIT;
  res.datas = &data;
  char topic[256] = "";
  rs232_EA900II_status_t *status = NULL;

  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  }

  // cmd process
  status = rs232_EA900II_get_status();
  if (status == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_EA900II_get_status failed", __LINE__, __func__);
    res.code = 2;
    goto ERROR;
  }

  res.datas->votagein = status->vIn;
  res.datas->votageout = status->vOut;
  res.datas->loadpercent = status->load;
  res.datas->freqin = status->freqIn;
  res.datas->votagebat = status->vBat;
  res.datas->temp = status->temp;
  res.datas->w0 = status->w0;
  res.datas->w1 = status->w1;
  res.datas->w2 = status->w2;
  res.datas->w3 = status->w3;
  res.datas->w4 = status->w4;
  res.datas->w5 = status->w5;
  res.datas->w6 = status->w6;
  res.datas->w7 = status->w7;

ERROR:
  res_size = mt__ea900_ii__data_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt__ea900_ii__data_res__pack(&res, res_buf);

  // marshal data
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt__ea900_ii__data_res__descriptor.name, msg,
      &frame_size);

  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == 0)
    ESP_LOGI(TAG,
             "%4d %s session:%lld get status success\n"
             "vIn:%3.2f vOut:%3.2f load:%3.2f freqIn:%3.2f "
             "vBat:%3.2f temp:%3.2f\n"
             "w0:%1d w1:%1d w2:%1d w3:%1d w4:%1d w5:%1d w6:%1d w7:%1d\n",
             __LINE__, __func__, msg->unary_call->session->value, status->vIn,
             status->vOut, status->load, status->freqIn, status->vBat,
             status->temp, status->w0, status->w1, status->w2, status->w3,
             status->w4, status->w5, status->w6, status->w7);
  else
    ESP_LOGE(TAG, "%4d %s session:%lld get status failed", __LINE__, __func__,
             msg->unary_call->session->value);

EXIT:
  free(res_buf);
  free(frame_buf);
  if (status != NULL)
    free(status);
  return;
}

static void
module_mqtt_process_get_model(Ai__Metathings__Component__DownStreamFrame *msg,
                              char *module_id) {
  esp_err_t err = ESP_OK;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;
  MtEA900II__ModelRes res = MT__EA900_II__MODEL_RES__INIT;
  MtEA900II__Model resmodel = MT__EA900_II__MODEL__INIT;
  res.model = &resmodel;
  char topic[256] = "";
  rs232_EA900II_model_t *model = NULL;

  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  }

  // cmd process
  model = rs232_EA900II_get_model();
  if (model == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_EA900II_get_model failed", __LINE__, __func__);
    res.code = 2;
    goto ERROR;
  }

  res.model->name = malloc(16);
  res.model->model = malloc(11);
  res.model->version = malloc(11);
  memcpy(res.model->name, model->name, 16);
  memcpy(res.model->model, model->model, 11);
  memcpy(res.model->version, model->version, 11);

ERROR:
  res_size = mt__ea900_ii__model_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt__ea900_ii__model_res__pack(&res, res_buf);

  // marshal data
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt__ea900_ii__model_res__descriptor.name, msg,
      &frame_size);

  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == 0)
    ESP_LOGI(TAG,
             "%4d %s session:%lld get model success\n"
             "name=%s\n"
             "model=%s\n"
             "version=%s",
             __LINE__, __func__, msg->unary_call->session->value, model->name,
             model->model, model->version);
  else
    ESP_LOGE(TAG, "%4d %s session:%lld get model failed", __LINE__, __func__,
             msg->unary_call->session->value);

EXIT:
  free(res_buf);
  free(frame_buf);
  if (model != NULL)
    free(model);
  if (res.model->name != NULL)
    free(res.model->name);
  if (res.model->model != NULL)
    free(res.model->model);
  if (res.model->version != NULL)
    free(res.model->version);
  return;
}

static void
module_mqtt_process_get_config(Ai__Metathings__Component__DownStreamFrame *msg,
                               char *module_id) {
  esp_err_t err = ESP_OK;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;
  MtEA900II__ConfigRes res = MT__EA900_II__CONFIG_RES__INIT;
  MtEA900II__Config resconfig = MT__EA900_II__CONFIG__INIT;
  res.config = &resconfig;
  char topic[256] = "";
  rs232_EA900II_config_t *config = NULL;

  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.code = 2;
    goto ERROR;
  }

  // cmd process
  config = rs232_EA900II_get_config();
  if (config == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_EA900II_get_config failed", __LINE__, __func__);
    res.code = 2;
    goto ERROR;
  }

  res.config->current = config->current;
  res.config->freq = config->freq;
  res.config->votage = config->votage;
  res.config->batvotage = config->batVotage;

ERROR:
  res_size = mt__ea900_ii__config_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt__ea900_ii__config_res__pack(&res, res_buf);

  // marshal data
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt__ea900_ii__config_res__descriptor.name, msg,
      &frame_size);

  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == 0)
    ESP_LOGI(TAG,
             "%4d %s session:%lld get config success\n"
             "votage=%3.2f\n"
             "current=%3.2f\n"
             "batvotage=%3.2f\n"
             "freq=%3.2f",
             __LINE__, __func__, msg->unary_call->session->value,
             config->votage, config->current, config->batVotage, config->freq);
  else
    ESP_LOGE(TAG, "%4d %s session:%lld get config failed", __LINE__, __func__,
             msg->unary_call->session->value);

EXIT:
  free(res_buf);
  free(frame_buf);
  if (config != NULL)
    free(config);
  return;
}

// global func  ===============================================================

void rs232_EA900II_module_mqtt_init() {
  mt_module_mqtt_add_handle(module_mqtt_process_get_status, "GetData");
  mt_module_mqtt_add_handle(module_mqtt_process_get_model, "GetModel");
  mt_module_mqtt_add_handle(module_mqtt_process_get_config, "GetConfig");
}
