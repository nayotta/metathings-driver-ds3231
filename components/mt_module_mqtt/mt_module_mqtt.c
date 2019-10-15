#include "string.h"

#include "esp_log.h"

#include "mt_module_flow_manage.h"
#include "mt_module_mqtt.h"
#include "mt_module_unarycall_ota.h"
#include "mt_mqtt_utils.h"

#include "google/protobuf/any.pb-c.h"
#include "google/protobuf/empty.pb-c.h"
#include "stream_frame.pb-c.h"

// global define ==============================================================
static const char *TAG = "MT_MODULE_MQTT";

extern char Module_id[128];
extern uint64_t Session_id;

mt_module_mqtt_t *app_handle = NULL;

// static func ================================================================
static void mt_module_mqtt_handle_unarycall(
    char *topic, Ai__Metathings__Component__DownStreamFrame *msg)
{
  uint64_t session_id = 0;

  if (mt_mqtt_utils_get_session_id_from_topic(topic, &session_id) != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s unexcept topic:%s", __LINE__, __func__, topic);
    goto EXIT;
  }

  if (session_id != Session_id)
  {
    ESP_LOGE(TAG, "%4d %s local session_id:%lld != recv session_id:%lld",
             __LINE__, __func__, Session_id, session_id);
    goto EXIT;
  }

  if (msg->unary_call == NULL)
  {
    ESP_LOGE(TAG, "%4d %s unarycall data is null", __LINE__, __func__);
    goto EXIT;
  }

  // ota api
  if (strcmp(msg->unary_call->method->value, "GetVersion") == 0)
  {
    mt_module_unarycall_version_handle(msg, Module_id);
    goto EXIT;
  }

  if (strcmp(msg->unary_call->method->value, "OtaUpdate") == 0)
  {
    mt_module_unarycall_ota_handle(msg, Module_id);
    goto EXIT;
  }

  // app api
  if (app_handle == NULL)
  {
    ESP_LOGW(TAG, "%4d %s no app handle", __LINE__, __func__);
  }
  else
  {
    for (int i = 0; i < app_handle->handle_size; i++)
    {
      if (strcmp(msg->unary_call->method->value, app_handle->methods[i]) == 0)
      {
        (*app_handle->handles[i])(msg, Module_id);
        goto EXIT;
      }
    }
  }

  ESP_LOGE(TAG, "%4d %s get unexcept method:%s", __LINE__, __func__,
           msg->unary_call->method->value);

EXIT:
  return;
}

static void mt_module_mqtt_handle_streamcall_config() {}

static void mt_module_mqtt_handle_streamcall_data() {}

static void mt_module_mqtt_handle_streamcall(
    char *topic, Ai__Metathings__Component__DownStreamFrame *msg)
{
  uint64_t session_id = 0;

  if (mt_mqtt_utils_get_session_id_from_topic(topic, &session_id) < 1)
  {
    ESP_LOGE(TAG, "%4d %s unexcept topic:%s", __LINE__, __func__, topic);
    goto EXIT;
  }

  if (session_id == 0)
  {
    ESP_LOGE(TAG, "%4d %s session_id is 0", __LINE__, __func__);
    goto EXIT;
  }

  if (session_id == Session_id)
  {
    mt_module_mqtt_handle_streamcall_config();
  }
  else
  {
    mt_module_mqtt_handle_streamcall_data();
  }

EXIT:
  return;
}

static void mt_module_mqtt_proxy_process(char *topic, void *buf, int size)
{
  Ai__Metathings__Component__DownStreamFrame *msg_mqtt;

  msg_mqtt =
      ai__metathings__component__down_stream_frame__unpack(NULL, size, buf);
  if (msg_mqtt == NULL)
  {
    ESP_LOGE(TAG, "%4d %s get null msg or unknown msg", __LINE__, __func__);
    return;
  }

  if (msg_mqtt->kind !=
      AI__METATHINGS__COMPONENT__STREAM_FRAME_KIND__STREAM_FRAME_KIND_USER)
  {
    ESP_LOGE(TAG, "%4d %s get unexcept steam frame type", __LINE__, __func__);
    goto EXIT;
  }

  switch (msg_mqtt->union_case)
  {
  case AI__METATHINGS__COMPONENT__UP_STREAM_FRAME__UNION_UNARY_CALL:
    mt_module_mqtt_handle_unarycall(topic, msg_mqtt);
    break;
  case AI__METATHINGS__COMPONENT__UP_STREAM_FRAME__UNION_STREAM_CALL:
    mt_module_mqtt_handle_streamcall(topic, msg_mqtt);
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

// global func ================================================================
void mt_module_mqtt_add_handle(
    void (*handle)(Ai__Metathings__Component__DownStreamFrame *msg,
                   char module_id[128]),
    char *method)
{
  ESP_LOGI(TAG, "%4d %s add handle method:%s", __LINE__, __func__, method);

  if (app_handle == NULL)
  {
    app_handle = malloc(sizeof(mt_module_mqtt_t));
    app_handle->handle_size = 1;
    app_handle->handles = (mt_module_mqtt_app_handle_t **)malloc(
        sizeof(mt_module_mqtt_app_handle_t *));
    app_handle->handles[0] = &handle;
    app_handle->methods = (char **)malloc(sizeof(char *));
    app_handle->methods[0] = malloc(strlen(method) + 1);
    memcpy(app_handle->methods[0], method, strlen(method) + 1);
  }
  else
  {
    mt_module_mqtt_t *temp_handle = app_handle;
    app_handle = malloc(sizeof(mt_module_mqtt_t));
    app_handle->handle_size = temp_handle->handle_size + 1;
    app_handle->handles = (mt_module_mqtt_app_handle_t **)malloc(
        sizeof(mt_module_mqtt_app_handle_t *) * app_handle->handle_size);
    app_handle->methods =
        (char **)malloc(sizeof(char *) * app_handle->handle_size);
    for (int i = 0; i < app_handle->handle_size - 1; i++)
    {
      app_handle->handles[i] = temp_handle->handles[i];
      app_handle->methods[i] = temp_handle->methods[i];
    }
    app_handle->handles[app_handle->handle_size - 1] = &handle;
    app_handle->methods[app_handle->handle_size - 1] =
        malloc(strlen(method) + 1);
    memcpy(app_handle->methods[app_handle->handle_size - 1], method,
           strlen(method) + 1);

    free(temp_handle->methods);
    free(temp_handle);
  }
}

void mt_module_mqtt_handle(char *topic, void *buf, int size)
{
  char *path = NULL;
  ESP_LOGI(TAG, "%4d %s get mqtt msg, topic:%s", __LINE__, __func__, topic);
  path = mt_mqtt_utils_get_path_from_topic(topic);
  if (path == NULL)
  {
    ESP_LOGE(TAG, "%4d %s get unexcept topic:%s", __LINE__, __func__, topic);
    return;
  }

  if (strcmp(path, "proxy") == 0)
  {
    ESP_LOGI(TAG, "%4d %s get proxy message", __LINE__, __func__);
    mt_module_mqtt_proxy_process(topic, buf, size);
    return;
  }

  if (strcmp(path, "flow_channel") == 0)
  {
    ESP_LOGI(TAG, "%4d %s get flow message", __LINE__, __func__);
    mt_module_flow_manage_mqtt_process(topic, buf, size);
    return;
  }

  ESP_LOGE(TAG, "%4d %s unexcept path recieve:%s", __LINE__, __func__, path);
  return;
}