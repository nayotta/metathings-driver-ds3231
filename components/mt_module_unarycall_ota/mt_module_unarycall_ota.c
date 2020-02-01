#include "esp_err.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_system.h"

#include "mt_module_unarycall_ota.h"
#include "mt_module_unarycall_utils.h"
#include "mt_mqtt_lan.h"
#include "mt_ota.h"

#include "google/protobuf/any.pb-c.h"
#include "google/protobuf/empty.pb-c.h"
#include "mt_proto_ota.pb-c.h"
#include "stream_frame.pb-c.h"

// global define ==============================================================
static const char *TAG = "MT_MODULE_UNARYCALL_OTA";

// global func ================================================================
void mt_module_unarycall_version_handle(
    Ai__Metathings__Component__DownStreamFrame *msg, char module_id[128]) {
  esp_err_t err = ESP_OK;
  GetVersionRes res = GET_VERSION_RES__INIT;
  res.code = 0;
  Version version = VERSION__INIT;
  res.version = &version;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;

  // cmd process
  esp_app_desc_t running_app_info;
  const esp_partition_t *running = esp_ota_get_running_partition();

  err = esp_ota_get_partition_description(running, &running_app_info);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s esp_ota_get_partition_description failed", __LINE__,
             __func__);
    res.code = 1;
  } else {
    res.version->version = running_app_info.version;
  }

  // marshall res
  res_size = get_version_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  get_version_res__pack(&res, res_buf);
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, get_version_res__descriptor.name, msg, &frame_size);

  // response
  char topic[256] = "";
  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  ESP_LOGI(TAG, "%4d %s session:%lld get version code:%d version:%s", __LINE__,
           __func__, msg->unary_call->session->value, res.code,
           res.version->version);

EXIT:
  free(res_buf);
  free(frame_buf);
  return;
}

void mt_module_unarycall_ota_handle(
    Ai__Metathings__Component__DownStreamFrame *msg, char module_id[128]) {
  esp_err_t err = ESP_OK;
  OtaUpdateReq *req = NULL;
  OtaUpdateRes res = OTA_UPDATE_RES__INIT;
  res.code = 0;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;

  // get req
  req = ota_update_req__unpack(NULL, msg->unary_call->value->value.len,
                               msg->unary_call->value->value.data);
  if (req == NULL) {
    ESP_LOGE(TAG, "%4d %s ota_update_req__unpack NULL", __LINE__, __func__);
    return;
  }

  // check req
  if (req->ota == NULL) {
    ESP_LOGE(TAG, "%4d %s req->ota NULL", __LINE__, __func__);
    return;
  }

  if (req->ota->url == NULL) {
    ESP_LOGE(TAG, "%4d %s req->ota->url NULL", __LINE__, __func__);
    return;
  }

  if (req->ota->url->value == NULL) {
    ESP_LOGE(TAG, "%4d %s req->ota->url->value NULL", __LINE__, __func__);
    return;
  }

  // cmd process
  mt_ota_t *ota = mt_ota_default_new(req->ota->url->value);
  err = mt_ota_task(ota);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s esp_ota_get_partition_description failed", __LINE__,
             __func__);
    res.code = 1;
  }

  // marshall res
  res_size = ota_update_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  ota_update_res__pack(&res, res_buf);
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, ota_update_res__descriptor.name, msg, &frame_size);

  // response
  char topic[256] = "";
  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  ESP_LOGI(TAG, "%4d %s session:%lld ota update, code:%d url:%s", __LINE__,
           __func__, msg->unary_call->session->value, res.code,
           req->ota->url->value);

EXIT:
  free(res_buf);
  free(frame_buf);
  return;
}
