#include "esp_log.h"

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "mt_module_flow.h"
#include "mt_module_flow_manage.h"
#include "mt_module_http.h"
#include "mt_module_http_utils.h"
#include "mt_mqtt_lan.h"
#include "mt_mqtt_utils.h"
#include "mt_nvs_config.h"

#include "google/protobuf/struct.pb-c.h"

#include "push_frame_to_flow.pb-c.h"

// global define ==============================================================

static const char *TAG = "MT_MODULE_FLOW";

// help func ==================================================================

mt_module_flow_struct_group_t *mt_module_flow_new_struct_group(int size) {
  if (size <= 0) {
    ESP_LOGE(TAG, "%4d %s size:%d error", __LINE__, __func__, size);
    return NULL;
  }

  mt_module_flow_struct_group_t *group =
      malloc(sizeof(mt_module_flow_struct_group_t));
  group->size = size;
  group->value = malloc(size * sizeof(mt_module_flow_struct_t *));
  for (int i = 0; i < size; i++) {
    group->value[i] = malloc(sizeof(mt_module_flow_struct_t));
    group->value[i]->key = NULL;
    group->value[i]->type = GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE;
    group->value[i]->bool_value = false;
  }

  return group;
}

mt_module_flow_struct_group_t *
mt_module_flow_new_struct_group_with_notify(int size) {
  char notify_str[] = "notify";
  if (size <= 0) {
    ESP_LOGE(TAG, "%4d %s size:%d error", __LINE__, __func__, size);
    return NULL;
  }
  mt_module_flow_struct_group_t *group =
      malloc(sizeof(mt_module_flow_struct_group_t));
  group = mt_module_flow_new_struct_group(size + 1);
  int notify_size = strlen(notify_str) + 1;
  group->value[size]->key = malloc(notify_size);
  memcpy(group->value[size]->key, notify_str, notify_size);
  group->value[size]->type = GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE;
  group->value[size]->bool_value = true;

  return group;
}

void mt_module_flow_free_struct_group(mt_module_flow_struct_group_t *group) {
  if (group == NULL)
    return;

  for (int i = 0; i < group->size; i++) {
    if (group->value[i] != NULL) {
      if (group->value[i]->key != NULL)
        free(group->value[i]->key);

      if (group->value[i]->type == GOOGLE__PROTOBUF__VALUE__KIND_STRING_VALUE) {
        if (group->value[i]->string_value != NULL)
          free(group->value[i]->string_value);
      }
      free(group->value[i]);
    }
  }
  if (group->value != NULL)
    free(group->value);

  free(group);
}

void mt_module_flow_set_bool_value(mt_module_flow_struct_t *struct_in,
                                   char *key, bool value_in) {
  int32_t key_size = 0;

  if (key == NULL) {
    key_size = 1;
  } else {
    key_size = strlen(key) + 1;
  }
  struct_in->key = malloc(key_size);
  memcpy(struct_in->key, key, key_size);

  struct_in->type = GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE;
  struct_in->bool_value = value_in;
}

void mt_module_flow_set_number_value(mt_module_flow_struct_t *struct_in,
                                     char *key, double value_in) {
  int32_t key_size = 0;

  if (key == NULL) {
    key_size = 1;
  } else {
    key_size = strlen(key) + 1;
  }
  struct_in->key = malloc(key_size);
  memcpy(struct_in->key, key, key_size);

  struct_in->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  struct_in->number_value = value_in;
}

void mt_module_flow_set_string_value(mt_module_flow_struct_t *struct_in,
                                     char *key, char *value_in) {
  int32_t key_size = 0;
  int32_t value_size = 0;

  if (key == NULL) {
    key_size = 1;
  } else {
    key_size = strlen(key) + 1;
  }
  struct_in->key = malloc(key_size);
  memcpy(struct_in->key, key, key_size);

  struct_in->type = GOOGLE__PROTOBUF__VALUE__KIND_STRING_VALUE;

  if (value_in == NULL) {
    value_size = 1;
  } else {
    value_size = strlen(value_in) + 1;
  }
  struct_in->string_value = malloc(value_size);
  memcpy(struct_in->string_value, value_in, value_size);
}

// global func ================================================================

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

  err = mqtt_pub_msg(ping_topic, frame_buf, frame_size);
  free(frame_buf);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    return;
  }

  ESP_LOGI(TAG, "%4d %s session:%s sent ping", __LINE__, __func__,
           module_flow->session);
  return;
}

static void module_flow_task_loop(mt_module_flow_t *module_flow) {
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

    flow_res = mt_module_http_actions_push_frame_to_flow(
        module_flow->module_http, module_flow->flow, module_flow->config_ack,
        module_flow->data_ack);
    if (flow_res == NULL) {
      ESP_LOGE(TAG, "%4d %s mt_module_http_actions_push_frame_to_flow failed",
               __LINE__, __func__);
    } else {
      ESP_LOGI(TAG,
               "%4d %s mt_module_http_actions_push_frame_to_flow success, "
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
    module_flow->ping_count++;
    vTaskDelay(module_flow->ping_interval / portTICK_PERIOD_MS);
  }
}

static void module_switch_flow_pong_process(mt_module_flow_t *module_flow) {
  module_flow->ping_count = 0;
  ESP_LOGI(TAG, "%4d %s session:%s get pong response", __LINE__, __func__,
           module_flow->session);
}

static void module_switch_flow_ack_process(mt_module_flow_t *module_flow,
                                           PushFrameToFlowResponse *frame_res) {
  if (frame_res->id == NULL) {
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
                            uint8_t *buf, uint8_t size) {
  PushFrameToFlowResponse *frame_res = NULL;

  frame_res = push_frame_to_flow_response__unpack(NULL, size, buf);
  if (frame_res == NULL) {
    ESP_LOGE(TAG, "%4d %s push_frame_to_flow_response__unpack get NULL",
             __LINE__, __func__);
    return;
  }

  switch (frame_res->response_case) {
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
    goto EXIT;
  }

EXIT:
  push_frame_to_flow_response__free_unpacked(frame_res, NULL);
  return;
}

void mt_module_flow_task(mt_module_flow_t *module_flow) {
  // arg check
  if (module_flow == NULL) {
    ESP_LOGE(TAG, "%4d %s flow_name NULL", __LINE__, __func__);
    return;
  } else {
    if (module_flow->module_http == NULL) {
      ESP_LOGE(TAG, "%4d %s module_flow->module_http NULL", __LINE__, __func__);
      return;
    } else {
      if (module_flow->module_http->module == NULL) {
        ESP_LOGE(TAG, "%4d %s module_flow->module_http->module NULL", __LINE__,
                 __func__);
        return;
      } else {
        if (module_flow->module_http->module->id == NULL) {
          ESP_LOGE(TAG, "%4d %s module_flow->module_http->module->id NULL",
                   __LINE__, __func__);
          return;
        }
      }
    }
  }

  if (module_flow->flow == NULL) {
    ESP_LOGE(TAG, "%4d %s module_flow->flow NULL", __LINE__, __func__);
    return;
  } else {
    if (module_flow->flow->name == NULL) {
      ESP_LOGE(TAG, "%4d %s module_flow->flow->name NULL", __LINE__, __func__);
      return;
    }
  }

  xTaskCreate((TaskFunction_t)module_flow_task_loop, "MT_MODULE_FLOW_TASK",
              4 * 1024, module_flow, 10, NULL);
}

mt_module_flow_t *mt_module_flow_new(int module_index, int flow_index,
                                     mt_module_http_t *module_http) {
  esp_err_t err = ESP_OK;
  mt_module_flow_t *module_flow = malloc(sizeof(mt_module_flow_t));
  char *flow_name = NULL;

  flow_name = mt_nvs_config_get_flow_name(module_index, flow_index);
  if (flow_name == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_get_flow_name module:%d flow:%d failed",
             __LINE__, __func__, module_index, flow_index);
    return NULL;
  }

  module_flow->module_index = module_index;
  module_flow->flow_index = flow_index;
  module_flow->module_http = module_http;
  module_flow->flow = malloc(sizeof(flow_t));
  module_flow->flow->name = flow_name;
  module_flow->session = NULL;
  module_flow->create_push_frame_interval = 30 * 1000; // 30s
  module_flow->push_frame_interval = 20 * 1000;        // 20s
  module_flow->ping_interval = 30 * 1000;              // 30s
  module_flow->ping_retry_times = 3; // retry 3 times, nor to restart
  module_flow->ping_count = 0;
  module_flow->push_ack = true;
  module_flow->config_ack = true;
  module_flow->data_ack = false;
  module_flow->data_id = NULL;

  err = mt_module_flow_manage_add(module_flow);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_manage_add module:%d flow:%d failed",
             __LINE__, __func__, module_index, flow_index);
    return NULL;
  }

  return module_flow;
}

uint8_t *mt_module_flow_pack_frame(mt_module_flow_struct_group_t *value_in,
                                   char *session_id, int *size_out) {
  uint8_t frame_num = value_in->size;
  uint8_t frame_count = 0;

  if (frame_num <= 0)
    return NULL;

  PushFrameToFlowRequest frame_req = PUSH_FRAME_TO_FLOW_REQUEST__INIT;

  // frame id
  Google__Protobuf__StringValue frame_id = GOOGLE__PROTOBUF__STRING_VALUE__INIT;
  frame_req.id = &frame_id;
  frame_req.id->value = session_id;
  frame_req.request_case = PUSH_FRAME_TO_FLOW_REQUEST__REQUEST_FRAME;

  // frame struct
  OpFrame frame = OP_FRAME__INIT;
  frame_req.frame = &frame;
  Google__Protobuf__Struct frame_data = GOOGLE__PROTOBUF__STRUCT__INIT;
  frame_req.frame->data = &frame_data;

  // struct data fields
  frame_req.frame->data->n_fields = frame_num;
  frame_req.frame->data->fields =
      malloc(frame_num * sizeof(Google__Protobuf__Struct__FieldsEntry *));

  for (int i = 0; i < frame_num; i++) {
    frame_req.frame->data->fields[frame_count] =
        malloc(sizeof(Google__Protobuf__Struct__FieldsEntry));
    google__protobuf__struct__fields_entry__init(
        frame_req.frame->data->fields[frame_count]);
    frame_req.frame->data->fields[frame_count]->key = value_in->value[i]->key;
    Google__Protobuf__Value *value_value =
        malloc(sizeof(Google__Protobuf__Value));
    google__protobuf__value__init(value_value);
    frame_req.frame->data->fields[frame_count]->value = value_value;
    frame_req.frame->data->fields[frame_count]->value->kind_case =
        value_in->value[i]->type;
    switch (value_in->value[i]->type) {
    case GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE:
      frame_req.frame->data->fields[frame_count]->value->number_value =
          value_in->value[i]->number_value;
      break;
    case GOOGLE__PROTOBUF__VALUE__KIND_STRING_VALUE: {
      int string_size = strlen(value_in->value[i]->string_value) + 1;
      frame_req.frame->data->fields[frame_count]->value->string_value =
          malloc(string_size);
      memcpy(frame_req.frame->data->fields[frame_count]->value->string_value,
             value_in->value[i]->string_value, string_size);
      break;
    }
    case GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE:
      frame_req.frame->data->fields[frame_count]->value->bool_value =
          value_in->value[i]->bool_value;
      break;
    default:
      ESP_LOGE(TAG, "%4d %s unexcept type:%d", __LINE__, __func__,
               value_in->value[i]->type);
      break;
    }
    frame_count++;
  }

  // pack frame
  *size_out = push_frame_to_flow_request__get_packed_size(&frame_req);
  uint8_t *frame_req_buf = malloc(*size_out);
  push_frame_to_flow_request__pack(&frame_req, frame_req_buf);

  // free buf
  for (int i = 0; i < frame_num; i++) {
    google__protobuf__value__free_unpacked(
        frame_req.frame->data->fields[i]->value, NULL);
    free(frame_req.frame->data->fields[i]);
  }
  free(frame_req.frame->data->fields);

  return frame_req_buf;
}

esp_err_t mt_module_flow_sent_msg(mt_module_flow_t *module_flow,
                                  mt_module_flow_struct_group_t *group) {
  esp_err_t err = ESP_OK;
  char req_topic[256] = "";
  int frame_req_size = 0;
  uint8_t *frame_req_buf = NULL;

  // check arg
  if (module_flow == NULL) {
    ESP_LOGE(TAG, "%4d %s module_flow NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  } else {
    if (module_flow->session == NULL) {
      ESP_LOGE(TAG, "%4d %s module_flow->session NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }

    if (module_flow->module_http == NULL) {
      ESP_LOGE(TAG, "%4d %s module_flow->module_http NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    } else {
      if (module_flow->module_http->module == NULL) {
        ESP_LOGE(TAG, "%4d %s module_flow->module_http->module NULL", __LINE__,
                 __func__);
        return ESP_ERR_INVALID_ARG;
      } else {
        if (module_flow->module_http->module->deviceID == NULL) {
          ESP_LOGE(TAG,
                   "%4d %s module_flow->module_http->module->deviceID NULL",
                   __LINE__, __func__);
          return ESP_ERR_INVALID_ARG;
        }
      }
    }
  }
  if (group == NULL) {
    ESP_LOGE(TAG, "%4d %s group NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  } else {
    if (group->size == 0) {
      ESP_LOGE(TAG, "%4d %s group->size  zero", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }
    for (int i = 0; i < group->size; i++) {
      if (group->value == NULL) {
        ESP_LOGE(TAG, "%4d %s group->value NULL", __LINE__, __func__);
        return ESP_ERR_INVALID_ARG;
      } else {
        if (group->value[i]->key == NULL) {
          ESP_LOGE(TAG, "%4d %s group->value[%d]->key NULL", __LINE__, __func__,
                   i);
          return ESP_ERR_INVALID_ARG;
        }
      }
    }
  }

  sprintf(req_topic, "mt/devices/%s/flow_channel/sessions/%s/upstream",
          module_flow->module_http->module->deviceID, module_flow->session);

  // marshall data
  frame_req_buf =
      mt_module_flow_pack_frame(group, module_flow->session, &frame_req_size);
  if (frame_req_buf == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_pack_frame error", __LINE__, __func__);
    goto EXIT;
  }

  // mqtt pub
  err = mqtt_pub_msg(req_topic, frame_req_buf, frame_req_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  // log
  ESP_LOGI(TAG, "%4d %s sent msg success", __LINE__, __func__);

EXIT:
  if (frame_req_buf != NULL) {
    free(frame_req_buf);
  }
  return err;
}
