#include "rs232_sim_air720h_flow.h"

#include "rs232_sim_air720h.h"
#include "rs232_sim_air720h_http.h"

#include "mt_module_http_utils.h"

#include "push_frame_to_flow.pb-c.h"

// static define ==============================================================

static const char *TAG = "RS232_SIM_AIR720H_FLOW";

// static func ================================================================

static void ping_once(mt_module_flow_t *module_flow) {
  esp_err_t err = ESP_OK;
  char ping_topic[256] = "";
  uint8_t frame_size = 0;
  uint8_t *frame_buf = NULL;

  if (module_flow == NULL) {
    ESP_LOGE(TAG, "%4d %s module_flow NULL", __LINE__, __func__);
    return;
  } else {
    if (module_flow->session == NULL) {
      ESP_LOGE(TAG, "%4d %s module_flow->session  NULL", __LINE__, __func__);
      return;
    }

    if (module_flow->module_http == NULL) {
      ESP_LOGE(TAG, "%4d %s module_flow->module_http  NULL", __LINE__,
               __func__);
      return;
    } else {
      if (module_flow->module_http->module == NULL) {
        ESP_LOGE(TAG, "%4d %s module_flow->module_http->module NULL", __LINE__,
                 __func__);
        return;
      } else {
        if (module_flow->module_http->module->deviceID == NULL) {
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

  err = rs232_sim_air720h_mqtt_pub(ping_topic, frame_buf, frame_size);
  free(frame_buf);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_mqtt_pub failed", __LINE__,
             __func__);
    return;
  }

  ESP_LOGI(TAG, "%4d %s session:%s sent ping", __LINE__, __func__,
           module_flow->session);
  return;
}

static void rs232_sim_air720h_flow_loop(mt_module_flow_t *module_flow) {
  push_frame_res_t *flow_res = NULL;

RESTART:
  // reset count
  module_flow->ping_count = 0;

  // http create push_flow session first
  while (true) {
    if (module_flow->session != NULL) {
      free(module_flow->session);
      module_flow->session = NULL;
    }

    flow_res = rs232_sim_air720h_http_push_frame_to_flow(
        module_flow->module_http, module_flow);
    if (flow_res == NULL) {
      ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_push_frame_to_flow failed",
               __LINE__, __func__);
    } else {
      ESP_LOGI(TAG,
               "%4d %s rs232_sim_air720h_http_push_frame_to_flow success, "
               "session:%s",
               __LINE__, __func__, flow_res->session_id);
      module_flow->session = malloc(strlen(flow_res->session_id) + 1);
      memcpy(module_flow->session, flow_res->session_id,
             strlen(flow_res->session_id) + 1);
      mt_module_http_utils_free_push_frame_res(flow_res);
      break;
    }

    vTaskDelay(module_flow->create_push_frame_interval / portTICK_PERIOD_MS);
  }

  // ping pong
  while (true) {
    if (module_flow->ping_count >= module_flow->ping_retry_times) {
      ESP_LOGE(TAG, "%4d %s session:%s ping reach max retry, restart loop",
               __LINE__, __func__, module_flow->session);
      vTaskDelay(2 * 1000 / portTICK_PERIOD_MS);
      goto RESTART;
    }

    ping_once(module_flow);
    // TODO(ZH) add pub msg process
    // module_flow->ping_count++;
    vTaskDelay(module_flow->ping_interval / portTICK_PERIOD_MS);
  }
}

// global func ================================================================

esp_err_t rs232_sim_air720h_flow_task(mt_module_flow_t *module_flow) {
  // arg check
  if (module_flow == NULL) {
    ESP_LOGE(TAG, "%4d %s flow_name NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  } else {
    if (module_flow->module_http == NULL) {
      ESP_LOGE(TAG, "%4d %s module_flow->module_http NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    } else {
      if (module_flow->module_http->module == NULL) {
        ESP_LOGE(TAG, "%4d %s module_flow->module_http->module NULL", __LINE__,
                 __func__);
        return ESP_ERR_INVALID_ARG;
      } else {
        if (module_flow->module_http->module->id == NULL) {
          ESP_LOGE(TAG, "%4d %s module_flow->module_http->module->id NULL",
                   __LINE__, __func__);
          return ESP_ERR_INVALID_ARG;
        }
      }
    }
  }
  if (module_flow->flow == NULL) {
    ESP_LOGE(TAG, "%4d %s module_flow->flow NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  } else {
    if (module_flow->flow->name == NULL) {
      ESP_LOGE(TAG, "%4d %s module_flow->flow->name NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }
  }

  xTaskCreate((TaskFunction_t)rs232_sim_air720h_flow_loop, "AIR720H_FLOW",
              4 * 1024, module_flow, 10, NULL);
  return ESP_OK;
}
