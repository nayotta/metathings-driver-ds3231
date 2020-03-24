#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "mt_module_unarycall_utils.h"
#include "stream_frame.pb-c.h"

#include "mt_proto_device_cloud_utils.h"

// global config ==============================================================
static const char *TAG = "MT_MOUDLE_UNARYCALL_UTILS";

// global func ================================================================
esp_err_t mt_module_unarycall_utils_check(
    Ai__Metathings__Component__DownStreamFrame *msg) {
  if (msg == NULL) {
    ESP_LOGE(TAG, "%4d %s msg NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (msg->union_case !=
      AI__METATHINGS__COMPONENT__DOWN_STREAM_FRAME__UNION_UNARY_CALL) {
    ESP_LOGE(TAG, "%4d %s msg->union_case != unarycall", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (msg->unary_call == NULL) {
    ESP_LOGE(TAG, "%4d %s msg->unary_call NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (msg->unary_call->value == NULL) {
    ESP_LOGE(TAG, "%4d %s msg->unary_call->value NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (msg->unary_call->session == NULL) {
    ESP_LOGE(TAG, "%4d %s msg->unary_call->session NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}

uint8_t *mt_module_unarycall_utils_pack(
    uint8_t *buf_in, int size_in, const char *methodres_in,
    Ai__Metathings__Component__DownStreamFrame *msg_in, int *frame_size_out) {
  uint8_t *frame_buf = NULL;
  char url[128] = "";

  Google__Protobuf__Any any = GOOGLE__PROTOBUF__ANY__INIT;
  sprintf(url, "%s%s", MT_PROTO_DEVICE_CLOUD_TYPE_URL_HEAD, methodres_in);
  any.type_url = url;
  any.value.data = buf_in;
  any.value.len = size_in;
  Ai__Metathings__Component__UpStreamFrame frame =
      AI__METATHINGS__COMPONENT__UP_STREAM_FRAME__INIT;
  frame.kind =
      AI__METATHINGS__COMPONENT__STREAM_FRAME_KIND__STREAM_FRAME_KIND_USER;
  frame.union_case =
      AI__METATHINGS__COMPONENT__UP_STREAM_FRAME__UNION_UNARY_CALL;
  Ai__Metathings__Component__UnaryCallValue unary =
      AI__METATHINGS__COMPONENT__UNARY_CALL_VALUE__INIT;
  frame.unary_call = &unary;
  frame.unary_call->session = msg_in->unary_call->session->value;
  frame.unary_call->method = msg_in->unary_call->method->value;
  frame.unary_call->value = &any;
  *frame_size_out =
      ai__metathings__component__up_stream_frame__get_packed_size(&frame);
  frame_buf = malloc(*frame_size_out);
  ai__metathings__component__up_stream_frame__pack(&frame, frame_buf);

  return frame_buf;
}
