#include "esp_log.h"

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "mt_module_flow.h"
#include "mt_module_http.h"
#include "mt_mqtt_lan.h"
#include "mt_mqtt_utils.h"

#include "google/protobuf/struct.pb-c.h"

#include "push_frame_to_flow.pb-c.h"

// global define ==============================================================
static const char *TAG = "MT_MODULE_FLOW";

// global func ================================================================
static void ping_once(mt_module_flow_t *module_flow)
{
  esp_err_t err = ESP_OK;
  char ping_topic[256] = "";
  uint8_t frame_size = 0;
  uint8_t *frame_buf = NULL;

  if (module_flow == NULL)
  {
    ESP_LOGE(TAG, "%4d %s module_flow NULL", __LINE__, __func__);
    return;
  }
  else
  {
    if (module_flow->session == NULL)
    {
      ESP_LOGE(TAG, "%4d %s module_flow->session  NULL", __LINE__, __func__);
      return;
    }

    if (module_flow->module_http == NULL)
    {
      ESP_LOGE(TAG, "%4d %s module_flow->module_http  NULL", __LINE__,
               __func__);
      return;
    }
    else
    {
      if (module_flow->module_http->module == NULL)
      {
        ESP_LOGE(TAG, "%4d %s module_flow->module_http->module NULL", __LINE__,
                 __func__);
        return;
      }
      else
      {
        if (module_flow->module_http->module->deviceID == NULL)
        {
          ESP_LOGE(TAG,
                   "%4d %s module_flow->module_http->module->deviceID NULL",
                   __LINE__, __func__);
          return;
        }
      }
    }
  }

  sprintf(ping_topic, "mt/devices/%s/flow_channel/sessions/%s/upstream",
          module_flow->module_http->module->deviceID, module_flow->session);

  // ping request
  PushFrameToFlowRequest frame = PUSH_FRAME_TO_FLOW_REQUEST__INIT;
  Google__Protobuf__StringValue frame_id = GOOGLE__PROTOBUF__STRING_VALUE__INIT;
  frame.id = &frame_id;
  frame.id->value = module_flow->session;
  frame.request_case = PUSH_FRAME_TO_FLOW_REQUEST__REQUEST_PING;
  PushFrameToFlowRequest__Ping ping = PUSH_FRAME_TO_FLOW_REQUEST__PING__INIT;
  frame.ping = &ping;
  frame_size = push_frame_to_flow_request__get_packed_size(&frame);
  frame_buf = malloc(frame_size);
  push_frame_to_flow_request__pack(&frame, frame_buf);

  err = mqtt_pub_msg(ping_topic, frame_buf, frame_size);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    return;
  }

  ESP_LOGI(TAG, "%4d %s session:%s sent ping", __LINE__, __func__,
           module_flow->session);
  return;
}

static void module_flow_task_loop(mt_module_flow_t *module_flow)
{
  push_frame_res_t *flow_res = NULL;

RESTART:
  // reset count
  module_flow->ping_count = 0;

  // http create push_flow session first
  while (true)
  {
    flow_res = mt_module_http_actions_push_frame_to_flow(
        module_flow->module_http, module_flow->flow, module_flow->config_ack,
        module_flow->data_ack);
    if (flow_res == NULL)
    {
      ESP_LOGE(TAG, "%4d %s mt_module_http_actions_push_frame_to_flow failed",
               __LINE__, __func__);
    }
    else
    {
      ESP_LOGI(TAG,
               "%4d %s mt_module_http_actions_push_frame_to_flow success, "
               "session:%s",
               __LINE__, __func__, flow_res->sesssion_id);
      module_flow->session = flow_res->sesssion_id;
      break;
    }

    vTaskDelay(module_flow->push_frame_interval / portTICK_PERIOD_MS);
  }

  // ping pong
  while (true)
  {
    if (module_flow->ping_count >= module_flow->ping_retry_times)
    {
      ESP_LOGE(TAG, "%4d %s session:%s ping reach max retry, restart loop",
               __LINE__, __func__, module_flow->session);
      vTaskDelay(2 * 1000 / portTICK_PERIOD_MS);
      goto RESTART;
    }

    ping_once(module_flow);
    module_flow->ping_count++;
    vTaskDelay(module_flow->ping_interval / portTICK_PERIOD_MS);
  }
}

static void module_switch_flow_pong_process(mt_module_flow_t *module_flow)
{
  module_flow->ping_count = 0;
  ESP_LOGI(TAG, "%4d %s session:%s get pong response", __LINE__, __func__,
           module_flow->session);
}

static void module_switch_flow_ack_process(mt_module_flow_t *module_flow,
                                           PushFrameToFlowResponse *frame_res)
{
  if (frame_res->id == NULL)
  {
    ESP_LOGE(TAG, "%4d %s frame_res->id  NULL", __LINE__, __func__);
    return;
  }

  if (module_flow->data_id != NULL)
    free(module_flow->data_id);

  module_flow->data_id = malloc(strlen(frame_res->id) + 1);
  module_flow->data_id[strlen(frame_res->id)] = '\0';
  memcpy(module_flow->data_id, frame_res->id, strlen(frame_res->id));

  module_flow->data_ack = true;

  ESP_LOGI(TAG, "%4d %s session:%s id:%s get ack", __LINE__, __func__,
           module_flow->session, module_flow->data_id);
  return;
}

void mt_module_flow_process(mt_module_flow_t *module_flow, char *topic,
                                uint8_t *buf, uint8_t size)
{
  PushFrameToFlowResponse *frame_res = NULL;

  frame_res = push_frame_to_flow_response__unpack(NULL, size, buf);
  if (frame_res == NULL)
  {
    ESP_LOGE(TAG, "%4d %s push_frame_to_flow_response__unpack get NULL",
             __LINE__, __func__);
    return;
  }

  switch (frame_res->response_case)
  {
  case PUSH_FRAME_TO_FLOW_RESPONSE__RESPONSE_PONG:
    module_switch_flow_pong_process(module_flow);
    break;
  case PUSH_FRAME_TO_FLOW_RESPONSE__RESPONSE_CONFIG:
    // use http, no need
    break;
  case PUSH_FRAME_TO_FLOW_RESPONSE__RESPONSE_ACK:
    module_switch_flow_ack_process(module_flow, frame_res);
    break;
  default:
    ESP_LOGE(TAG, "%4d %s unexcept frame_res->response_case:%d", __LINE__,
             __func__, frame_res->response_case);
    return;
  }
}

void mt_module_flow_task(mt_module_flow_t *module_flow, char *task_name)
{
  // arg check
  if (module_flow == NULL)
  {
    ESP_LOGE(TAG, "%4d %s flow_name NULL", __LINE__, __func__);
    return;
  }
  else
  {
    if (module_flow->module_http == NULL)
    {
      ESP_LOGE(TAG, "%4d %s module_flow->module_http NULL", __LINE__, __func__);
      return;
    }
    else
    {
      if (module_flow->module_http->module == NULL)
      {
        ESP_LOGE(TAG, "%4d %s module_flow->module_http->module NULL", __LINE__,
                 __func__);
        return;
      }
      else
      {
        if (module_flow->module_http->module->id == NULL)
        {
          ESP_LOGE(TAG, "%4d %s module_flow->module_http->module->id NULL",
                   __LINE__, __func__);
          return;
        }
      }
    }
  }

  if (module_flow->flow == NULL)
  {
    ESP_LOGE(TAG, "%4d %s module_flow->flow NULL", __LINE__, __func__);
    return;
  }
  else
  {
    if (module_flow->flow->name == NULL)
    {
      ESP_LOGE(TAG, "%4d %s module_flow->flow->name NULL", __LINE__, __func__);
      return;
    }
  }

  xTaskCreate((TaskFunction_t)module_flow_task_loop, task_name, 8 * 1024,
              module_flow, 10, NULL);
}

mt_module_flow_t *mt_module_flow_new()
{
  mt_module_flow_t *module_flow = malloc(sizeof(mt_module_flow_t));

  module_flow->module_http = NULL;
  module_flow->flow = NULL;
  module_flow->session = NULL;
  module_flow->create_push_frame_interval = 30 * 1000; // 30s
  module_flow->push_frame_interval = 10 * 1000;        // 10s
  module_flow->ping_interval = 30 * 1000;              // 30s
  module_flow->ping_retry_times = 3;                   // retry 3 times, nor to restart
  module_flow->ping_count = 0;
  module_flow->push_ack = true;
  module_flow->config_ack = true;
  module_flow->data_ack = false;
  module_flow->data_id = NULL;

  return module_flow;
}