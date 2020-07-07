#include "string.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "mt_module_http_manage.h"
#include "mt_module_lora.h"
#include "mt_module_mqtt.h"
#include "mt_module_unarycall_ota.h"
#include "mt_module_unarycall_utils.h"
#include "mt_mqtt.h"
#include "mt_mqtt_lan.h"
#include "mt_mqtt_utils.h"
#include "mt_nvs_config.h"
#include "mt_nvs_storage.h"
#include "mt_utils.h"

#include "google/protobuf/any.pb-c.h"
#include "google/protobuf/empty.pb-c.h"
#include "stream_frame.pb-c.h"

#include "rs232_lora_ebyte.h"
#include "rs232_lora_ebyte_module_manage.h"

// global define ==============================================================

static const char *TAG = "MT_MODULE_LORA";

extern char Module_id[128];
extern uint64_t Session_id;

// static func ================================================================

static void mt_module_lora_handle_streamcall_config() {}

static void mt_module_lora_handle_streamcall_data() {}

static void mt_module_lora_handle_streamcall(char *topic, uint8_t *buf,
                                             int size) {
  uint64_t session_id = 0;

  if (mt_mqtt_utils_get_session_id_from_topic(topic, &session_id) < 1) {
    ESP_LOGE(TAG, "%4d %s unexcept topic:%s", __LINE__, __func__, topic);
    goto EXIT;
  }

  if (session_id == 0) {
    ESP_LOGE(TAG, "%4d %s session_id is 0", __LINE__, __func__);
    goto EXIT;
  }

  if (session_id == Session_id) {
    mt_module_lora_handle_streamcall_config();
  } else {
    mt_module_lora_handle_streamcall_data();
  }

EXIT:
  return;
}

static void
mt_module_lora_handle_unarycall_app_task(mt_module_lora_t *lora_msg) {
  esp_err_t err = ESP_OK;
  rs232_lora_ebyte_data_t *ebyte_data = NULL;
  rs232_lora_ebyte_data_t *ebyte_out = NULL;
  char *up_topic = NULL;
  Ai__Metathings__Component__DownStreamFrame *msg = NULL;

  msg = ai__metathings__component__down_stream_frame__unpack(
      NULL, lora_msg->size, lora_msg->buf);
  if (msg == NULL) {
    ESP_LOGE(TAG, "%4d %s get null msg or unknown msg", __LINE__, __func__);
    goto EXIT;
  }

  ebyte_data = rs232_lora_ebyte_new_data();
  ebyte_data->id = 2;
  ebyte_data->cmd = RS232_LORA_EBYTE_CMD_TYPE_UNARYCALL;
  ebyte_data->type = 2;
  ebyte_data->data = malloc(lora_msg->size);
  memcpy(ebyte_data->data, lora_msg->buf, lora_msg->size);
  ebyte_data->len = lora_msg->size;

  ebyte_out = rs232_lora_ebyte_sent_and_wait_finish(ebyte_data);
  // TODO(zh) need response timeout and error
  if (ebyte_out == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_sent_and_wait_finish return NULL",
             __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto ERROR;
  }
  ESP_LOGW(TAG, "%4d %s session=%lld id=%d cmd=%d type=%d data=%p len=%d",
           __LINE__, __func__, msg->unary_call->session->value, ebyte_out->id,
           ebyte_out->cmd, ebyte_out->type, ebyte_out->data, ebyte_out->len);

  char *session_str = NULL;
  session_str = mt_utils_int64_to_string(msg->unary_call->session->value);
  up_topic = mt_mqtt_utils_convert_path_unarycall_req_to_res(lora_msg->topic,
                                                             session_str);
  if (session_str != NULL)
    free(session_str);
  if (up_topic == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_mqtt_utils_set_path_downstream_to_upstream null",
             __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto ERROR;
  }

ERROR:
  if (err != ESP_OK) {
    uint8_t *err_buf = NULL;
    int32_t err_size = 0;
    err_buf = mt_module_unarycall_utils_pack(
        NULL, 0, msg->unary_call->method->value, msg, &err_size);
    if (err_buf == NULL) {
      ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_pack failed", __LINE__,
               __func__);
      goto EXIT;
    }

    err = mt_mqtt_pub_msg(up_topic, err_buf, err_size);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s mt_mqtt_pub_msgs failed", __LINE__, __func__);
      goto EXIT;
    }
  } else {
    err = mt_mqtt_pub_msg(up_topic, ebyte_out->data, ebyte_out->len);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s mt_mqtt_pub_msgs failed", __LINE__, __func__);
      goto EXIT;
    }
  }

EXIT:
  mt_module_lora_free(lora_msg);
  if (up_topic != NULL) {
    free(up_topic);
  }
  ai__metathings__component__down_stream_frame__free_unpacked(msg, NULL);
  rs232_lora_ebyte_free_data(ebyte_data);
  vTaskDelete(NULL);
};

mt_module_lora_t *mt_module_lora_copy_msg_to_lora(char *topic, uint8_t *buf,
                                                  int size) {
  mt_module_lora_t *lora_msg = malloc(sizeof(mt_module_lora_t));
  lora_msg->topic = NULL;
  lora_msg->buf = NULL;
  lora_msg->size = 0;

  lora_msg->topic = malloc(strlen(topic) + 1);
  memcpy(lora_msg->topic, topic, strlen(topic) + 1);
  if (size > 0) {
    lora_msg->buf = malloc(size);
    memcpy(lora_msg->buf, buf, size);
    lora_msg->size = size;
  }

  return lora_msg;
}

static void mt_module_lora_handle_unarycall(char *topic, uint8_t *buf,
                                            int size) {
  uint64_t session_id = 0;
  Ai__Metathings__Component__DownStreamFrame *msg = NULL;

  msg = ai__metathings__component__down_stream_frame__unpack(NULL, size, buf);
  if (msg == NULL) {
    ESP_LOGE(TAG, "%4d %s get null msg or unknown msg", __LINE__, __func__);
    return;
  }

  if (mt_mqtt_utils_get_session_id_from_topic(topic, &session_id) != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s unexcept topic:%s", __LINE__, __func__, topic);
    goto EXIT;
  }

  if (session_id != Session_id) {
    ESP_LOGE(TAG, "%4d %s local session_id:%lld != recv session_id:%lld",
             __LINE__, __func__, Session_id, session_id);
    goto EXIT;
  }

  if (msg->unary_call == NULL) {
    ESP_LOGE(TAG, "%4d %s unarycall data is null", __LINE__, __func__);
    goto EXIT;
  }

  // ota api
  if (strcmp(msg->unary_call->method->value, "GetVersion") == 0) {
    mt_module_unarycall_version_handle(msg, Module_id);
    goto EXIT;
  }

  if (strcmp(msg->unary_call->method->value, "OtaUpdate") == 0) {
    mt_module_unarycall_ota_handle(msg, Module_id);
    goto EXIT;
  }

  // app api
  mt_module_lora_t *lora_msg = NULL;
  lora_msg = mt_module_lora_copy_msg_to_lora(topic, buf, size);
  xTaskCreate((TaskFunction_t)mt_module_lora_handle_unarycall_app_task,
              "UNARYCALL_TASK", 8 * 1024, lora_msg, 10, NULL);

EXIT:
  ai__metathings__component__down_stream_frame__free_unpacked(msg, NULL);
  return;
}

static void mt_module_lora_proxy_process(char *topic, uint8_t *buf, int size) {
  Ai__Metathings__Component__DownStreamFrame *msg_mqtt;

  msg_mqtt =
      ai__metathings__component__down_stream_frame__unpack(NULL, size, buf);
  if (msg_mqtt == NULL) {
    ESP_LOGE(TAG, "%4d %s get null msg or unknown msg", __LINE__, __func__);
    return;
  }

  if (msg_mqtt->kind !=
      AI__METATHINGS__COMPONENT__STREAM_FRAME_KIND__STREAM_FRAME_KIND_USER) {
    ESP_LOGE(TAG, "%4d %s get unexcept steam frame type", __LINE__, __func__);
    goto EXIT;
  }

  switch (msg_mqtt->union_case) {
  case AI__METATHINGS__COMPONENT__UP_STREAM_FRAME__UNION_UNARY_CALL:
    ESP_LOGI(TAG, "%4d %s handle unarycall", __LINE__, __func__);
    mt_module_lora_handle_unarycall(topic, buf, size);
    break;
  case AI__METATHINGS__COMPONENT__UP_STREAM_FRAME__UNION_STREAM_CALL:
    ESP_LOGI(TAG, "%4d %s handle streamcall", __LINE__, __func__);
    mt_module_lora_handle_streamcall(topic, buf, size);
    break;
  default:
    ESP_LOGE(TAG, "%4d %s get unexcepted stream frame type", __LINE__,
             __func__);
    goto EXIT;
  }

EXIT:
  ai__metathings__component__down_stream_frame__free_unpacked(msg_mqtt, NULL);
  return;
}

// help func ==================================================================

esp_err_t mt_module_lora_get_client_id(int32_t *id) {
  if (mt_nvs_read_int32_config("lora_id", id) == false) {
    ESP_LOGE(TAG, "%4d %s read lora_id failed", __LINE__, __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

void mt_module_lora_free(mt_module_lora_t *msg) {
  if (msg == NULL)
    return;

  if (msg->buf != NULL)
    free(msg->buf);

  free(msg);
}

void mt_module_lora_update_session(uint64_t session_id) {
  Session_id = session_id;
}

// global func ================================================================

void mt_module_lora_handle(char *topic, void *buf, int size) {
  char *path = NULL;
  ESP_LOGI(TAG, "%4d %s get mqtt msg, topic:%s", __LINE__, __func__, topic);
  path = mt_mqtt_utils_get_path_from_topic(topic);
  if (path == NULL) {
    ESP_LOGE(TAG, "%4d %s get unexcept topic:%s", __LINE__, __func__, topic);
    return;
  }

  if (strcmp(path, "proxy") == 0) {
    ESP_LOGI(TAG, "%4d %s get proxy message", __LINE__, __func__);
    mt_module_lora_proxy_process(topic, (uint8_t *)buf, size);
    return;
  }

  if (strcmp(path, "flow_channel") == 0) {
    // TODO(zh) flow process
  }

  ESP_LOGE(TAG, "%4d %s unexcept path recieve:%s", __LINE__, __func__, path);
  return;
}
