#include "esp_err.h"
#include "esp_log.h"

#include "google/protobuf/any.pb-c.h"
#include "google/protobuf/empty.pb-c.h"
#include "mt_proto_aew100.pb-c.h"
#include "stream_frame.pb-c.h"

#include "modbus_aew100.h"
#include "modbus_aew100_utils.h"
#include "mt_module_flow.h"
#include "mt_module_flow_manage.h"
#include "mt_module_unarycall_utils.h"
#include "mt_mqtt_lan.h"
#include "mt_mqtt_utils.h"
#include "mt_utils_error.h"
#include "mt_module_mqtt.h"

// global config ==============================================================

static const char *TAG = "MODULE_AWE100_MQTT";

static int32_t module_index = 1;

// static  func ===============================================================

static void module_aew100_mqtt_process_get_current(
    Ai__Metathings__Component__DownStreamFrame *msg, char *module_id) {
  esp_err_t err = ESP_OK;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  MtAew100__GetCurrentReq *req = NULL;
  MtAew100__GetCurrentRes res = MT_AEW100__GET_CURRENT_RES__INIT;
  modbus_aew100_get_current_res_init(&res);
  MtAew100__CurrentData current = MT_AEW100__CURRENT_DATA__INIT;
  modbus_aew100_current_data_init(&current);
  res.currentdata = &current;
  int32_t virt_addr = 0;
  int32_t real_addr = 0;

  // arg checks
  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.rescode = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // get req
  req = mt_aew100__get_current_req__unpack(NULL,
                                           msg->unary_call->value->value.len,
                                           msg->unary_call->value->value.data);
  err = modbus_aew100_utils_check_get_current_req(&virt_addr, req);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_aew100_util_check_get_current_req failed",
             __LINE__, __func__);
    res.rescode = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // conver addr
  err = modbus_aew100_utils_convert_virt_addr_to_real_addr(
      module_index, virt_addr, &real_addr);
  if (err != ESP_OK) {
    ESP_LOGE(TAG,
             "%4d %s modbus_aew100_utils_convert_virt_addr_to_real_addr failed",
             __LINE__, __func__);
    res.rescode = MT_ERR_INVALID_RESPONSE;
    goto ERROR;
  }

  // get current process
  err = mt_aew100_get_current_ABC(real_addr, &current.currenta,
                                  &current.currentb, &current.currentc);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_aew100_get_current_ABC failed", __LINE__,
             __func__);
    res.rescode = MT_ERR_INVALID_RESPONSE;
    goto ERROR;
  }

ERROR:
  // response
  res_size = mt_aew100__get_current_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_aew100__get_current_res__pack(&res, res_buf);
  err = mt_module_unarycall_utils_mqtt_sent_msg(
      res_buf, res_size, module_id, mt_aew100__get_current_req__descriptor.name,
      msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.rescode == MT_ERR_NO_ERR) {
    ESP_LOGI(TAG, "%4d %s GetCurrent virt_addr:%d real_addr:%d success",
             __LINE__, __func__, virt_addr, real_addr);
    ESP_LOGI(TAG, "%4d %s cuurentA:%f currentB:%f currentC:%f", __LINE__,
             __func__, current.currenta, current.currentb, current.currentc);
  } else {
    ESP_LOGE(TAG, "%4d %s GetCurrent virt_addr:%d real_addr:%d failed",
             __LINE__, __func__, virt_addr, real_addr);
  }

EXIT:
  if (req != NULL)
    mt_aew100__get_current_req__free_unpacked(req, NULL);
  if (res_buf != NULL)
    free(res_buf);
  return;
}

static void module_aew100_mqtt_process_get_data(
    Ai__Metathings__Component__DownStreamFrame *msg, char *module_id) {
  esp_err_t err = ESP_OK;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  MtAew100__GetDataReq *req = NULL;
  MtAew100__GetDataRes res = MT_AEW100__GET_DATA_RES__INIT;
  modbus_aew100_get_data_res_init(&res);
  MtAew100__Data data = MT_AEW100__DATA__INIT;
  res.data = &data;
  int32_t virt_addr = 0;
  int32_t real_addr = 0;

  // arg checks
  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.code = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // get req
  req = mt_aew100__get_data_req__unpack(NULL, msg->unary_call->value->value.len,
                                        msg->unary_call->value->value.data);
  err = modbus_aew100_utils_check_get_data_req(&virt_addr, req);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_aew100_utils_check_get_data_req failed",
             __LINE__, __func__);
    res.code = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // conver addr
  err = modbus_aew100_utils_convert_virt_addr_to_real_addr(
      module_index, virt_addr, &real_addr);
  if (err != ESP_OK) {
    ESP_LOGE(TAG,
             "%4d %s modbus_aew100_utils_convert_virt_addr_to_real_addr failed",
             __LINE__, __func__);
    res.code = MT_ERR_INVALID_RESPONSE;
    goto ERROR;
  }

  // get data process
  err = mt_aew100_get_data(real_addr, &data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_aew100_get_data failed", __LINE__, __func__);
    res.code = MT_ERR_INVALID_RESPONSE;
    goto ERROR;
  }

ERROR:
  // response
  res_size = mt_aew100__get_data_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_aew100__get_data_res__pack(&res, res_buf);
  err = mt_module_unarycall_utils_mqtt_sent_msg(
      res_buf, res_size, module_id, mt_aew100__get_data_req__descriptor.name,
      msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == MT_ERR_NO_ERR) {
    ESP_LOGI(TAG, "%4d %s GetData virt_addr:%d real_addr:%d success", __LINE__,
             __func__, virt_addr, real_addr);
    ESP_LOGI(TAG, "%4d %s currentA:%f currentB:%f currentC:%f", __LINE__,
             __func__, data.currenta, data.currentb, data.currentc);
    ESP_LOGI(TAG, "%4d %s voltageA:%f voltageB:%f voltageC:%f", __LINE__,
             __func__, data.votagea, data.votageb, data.votagec);
    ESP_LOGI(TAG, "%4d %s tempa:%f tempB:%f tempC:%f", __LINE__, __func__,
             data.tempa, data.tempb, data.tempc);
  } else {
    ESP_LOGE(TAG, "%4d %s GetData virt_addr:%d real_addr:%d failed", __LINE__,
             __func__, virt_addr, real_addr);
  }

EXIT:
  if (req != NULL)
    mt_aew100__get_data_req__free_unpacked(req, NULL);
  if (res_buf != NULL)
    free(res_buf);
  return;
}

// global func ================================================================

void modbus_aew100_module_mqtt_init() {
  mt_module_mqtt_add_handle(module_aew100_mqtt_process_get_current,
                            "GetCurrent");
  mt_module_mqtt_add_handle(module_aew100_mqtt_process_get_data, "GetData");
}
