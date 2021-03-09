#include "esp_log.h"
#include "string.h"

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "cJSON.h"

#include "mt_module_flow.h"
#include "mt_module_flow_manage.h"
#include "mt_module_http.h"
#include "mt_module_http_utils.h"
#include "mt_mqtt.h"
#include "mt_mqtt_utils.h"
#include "mt_nvs_config.h"
#include "mt_utils_login.h"
#include "mt_utils_string.h"

#include "google/protobuf/struct.pb-c.h"

#include "push_frame_to_flow.pb-c.h"

// global define ==============================================================

static const char *TAG = "MT_MODULE_FLOW";

// help func ==================================================================

mt_module_flow_struct_t *mt_module_flow_new_struct() {
  mt_module_flow_struct_t *struct_out = malloc(sizeof(mt_module_flow_struct_t));

  struct_out->key = NULL;
  struct_out->type = GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE;
  struct_out->bool_value = false;
  struct_out->use_default = true;
  struct_out->default_bool_value = false;

  return struct_out;
}

void mt_module_flow_free_struct(mt_module_flow_struct_t *struct_in) {
  if (struct_in == NULL)
    return;

  if (struct_in->type == GOOGLE__PROTOBUF__VALUE__KIND_STRING_VALUE) {
    if (struct_in->string_value != NULL)
      free(struct_in->string_value);
    if (struct_in->default_string_value != NULL)
      free(struct_in->default_string_value);
  }

  if (struct_in->key != NULL)
    free(struct_in->key);

  free(struct_in);
}

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
    group->value[i] = mt_module_flow_new_struct();
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
    mt_module_flow_free_struct(group->value[i]);
    group->value[i] = NULL;
  }

  if (group->value != NULL) {
    free(group->value);
  }

  free(group);
}

mt_module_flow_struct_group_t *
mt_module_flow_add_struct_to_group(mt_module_flow_struct_group_t *group_in,
                                   mt_module_flow_struct_t *struct_in) {
  mt_module_flow_struct_group_t *group_out = NULL;
  uint32_t out_size = 0;
  uint32_t in_size = 0;
  esp_err_t err = ESP_OK;

  if (group_in != NULL) {
    in_size = group_in->size;
  }

  out_size = in_size + 1;
  group_out = mt_module_flow_new_struct_group(out_size);
  group_out->size = out_size;
  for (int i = 0; i < out_size - 1; i++) {
    err = mt_module_flow_copy_struct(group_in->value[i], group_out->value[i]);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s mt_module_flow_copy_struct failed", __LINE__,
               __func__);
      goto EXIT;
    }
  }
  err = mt_module_flow_copy_struct(struct_in, group_out->value[out_size - 1]);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_copy_struct failed", __LINE__,
             __func__);
    goto EXIT;
  }

EXIT:
  if (err != ESP_OK) {
    mt_module_flow_free_struct_group(group_out);
    group_out = NULL;
  }
  mt_module_flow_free_struct_group(group_in);

  return group_out;
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

esp_err_t mt_module_flow_diff(mt_module_flow_struct_group_t *data1,
                              mt_module_flow_struct_group_t *data2,
                              bool *change) {
  *change = false;

  if (data1 == NULL) {
    ESP_LOGE(TAG, "%4d %s data1 NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (data2 == NULL) {
    ESP_LOGE(TAG, "%4d %s data2 NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (data1->size <= 0) {
    ESP_LOGE(TAG, "%4d %s data1->size:%d error", __LINE__, __func__,
             data1->size);
    return ESP_ERR_INVALID_ARG;
  }

  if (data1->size != data2->size) {
    ESP_LOGE(TAG, "%4d %s data1->size:%d != data2->size:%d ", __LINE__,
             __func__, data1->size, data2->size);
    return ESP_ERR_INVALID_ARG;
  }

  for (int i = 0; i < data1->size; i++) {
    if (data1->value[i]->key == NULL || data2->value[i]->key == NULL) {
      ESP_LOGE(TAG, "%4d %s key NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }
    if (strcmp(data1->value[i]->key, data2->value[i]->key) != 0) {
      ESP_LOGE(TAG, "%4d %s data1->key:%s != data2->key:%s ", __LINE__,
               __func__, data1->value[i]->key, data2->value[i]->key);
      return ESP_ERR_INVALID_ARG;
    }
    if (data1->value[i]->type != data2->value[i]->type) {
      ESP_LOGE(TAG, "%4d %s data1->type:%d != data2->type:%d", __LINE__,
               __func__, data1->value[i]->type, data2->value[i]->type);
      return ESP_ERR_INVALID_ARG;
    }

    switch (data1->value[i]->type) {
    case GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE:
      if (data1->value[i]->bool_value != data2->value[i]->bool_value) {
        *change = true;
      }
      break;
    case GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE:
      if (data1->value[i]->number_value != data2->value[i]->number_value) {
        *change = true;
      }
      break;
    case GOOGLE__PROTOBUF__VALUE__KIND_STRING_VALUE:
      if (strcmp(data1->value[i]->string_value,
                 data2->value[i]->string_value) != 0) {
        *change = true;
      }
      break;
    default:
      ESP_LOGE(TAG, "%4d %s unexcept type:%d", __LINE__, __func__,
               data1->value[i]->type);
      return ESP_ERR_INVALID_ARG;
      break;
    }
  }
  return ESP_OK;
}

mt_module_flow_struct_group_t *
mt_module_flow_get_diff(mt_module_flow_struct_group_t *data1,
                        mt_module_flow_struct_group_t *data2, bool *change) {
  esp_err_t err = ESP_OK;
  *change = false;
  mt_module_flow_struct_group_t *group_out = NULL;

  if (data1 == NULL) {
    ESP_LOGE(TAG, "%4d %s data1 NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (data2 == NULL) {
    ESP_LOGE(TAG, "%4d %s data2 NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (data1->size <= 0) {
    ESP_LOGE(TAG, "%4d %s data1->size:%d error", __LINE__, __func__,
             data1->size);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (data1->size != data2->size) {
    ESP_LOGE(TAG, "%4d %s data1->size:%d != data2->size:%d ", __LINE__,
             __func__, data1->size, data2->size);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  for (int i = 0; i < data1->size; i++) {
    if (data1->value[i]->key == NULL || data2->value[i]->key == NULL) {
      ESP_LOGE(TAG, "%4d %s key NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
    if (data1->value[i]->use_default == false) {
      group_out =
          mt_module_flow_add_struct_to_group(group_out, data1->value[i]);
    }
    if (strcmp(data1->value[i]->key, data2->value[i]->key) != 0) {
      ESP_LOGE(TAG, "%4d %s data1->key:%s != data2->key:%s ", __LINE__,
               __func__, data1->value[i]->key, data2->value[i]->key);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
    if (data1->value[i]->type != data2->value[i]->type) {
      ESP_LOGE(TAG, "%4d %s data1->type:%d != data2->type:%d", __LINE__,
               __func__, data1->value[i]->type, data2->value[i]->type);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }

    switch (data1->value[i]->type) {
    case GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE:
      if (data1->value[i]->bool_value != data2->value[i]->bool_value) {
        *change = true;
        group_out =
            mt_module_flow_add_struct_to_group(group_out, data1->value[i]);
      }
      break;
    case GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE:
      if (data1->value[i]->number_value != data2->value[i]->number_value) {
        *change = true;
        group_out =
            mt_module_flow_add_struct_to_group(group_out, data1->value[i]);
      }
      break;
    case GOOGLE__PROTOBUF__VALUE__KIND_STRING_VALUE:
      if (strcmp(data1->value[i]->string_value,
                 data2->value[i]->string_value) != 0) {
        *change = true;
        group_out =
            mt_module_flow_add_struct_to_group(group_out, data1->value[i]);
      }
      break;
    default:
      ESP_LOGE(TAG, "%4d %s unexcept type:%d", __LINE__, __func__,
               data1->value[i]->type);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
      break;
    }
  }
EXIT:
  if (err != ESP_OK) {
    mt_module_flow_free_struct_group(group_out);
    group_out = NULL;
  }
  return group_out;
}

esp_err_t mt_module_flow_copy_struct(mt_module_flow_struct_t *struct_in,
                                     mt_module_flow_struct_t *struct_out) {
  if (struct_in == NULL) {
    return ESP_OK;
  }

  struct_out->key = mt_utils_string_copy(struct_in->key);
  struct_out->type = struct_in->type;
  switch (struct_out->type) {
  case GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE:
    struct_out->bool_value = struct_in->bool_value;
    struct_out->default_bool_value = struct_in->default_bool_value;
    break;
  case GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE:
    struct_out->number_value = struct_in->number_value;
    struct_out->default_double_value = struct_in->default_double_value;
    break;
  case GOOGLE__PROTOBUF__VALUE__KIND_STRING_VALUE:
    struct_out->string_value = mt_utils_string_copy(struct_in->string_value);
    struct_out->default_string_value =
        mt_utils_string_copy(struct_in->default_string_value);
    break;
  default:
    ESP_LOGE(TAG, "%4d %s unexcept type:%d", __LINE__, __func__,
             struct_out->type);
    return ESP_ERR_INVALID_ARG;
    break;
  }
  return ESP_OK;
}

mt_module_flow_struct_group_t *
mt_module_flow_add_notify_to_group(mt_module_flow_struct_group_t *group_in) {
  mt_module_flow_struct_t *notify_struct = mt_module_flow_new_struct();
  mt_module_flow_struct_group_t *group_out = NULL;

  notify_struct->key = malloc(strlen("notify") + 1);
  memcpy(notify_struct->key, "notify", strlen("notify") + 1);
  notify_struct->bool_value = true;
  notify_struct->use_default = false;

  group_out = mt_module_flow_add_struct_to_group(group_in, notify_struct);

  mt_module_flow_free_struct(notify_struct);

  return group_out;
}

char *mt_utils_login_get_push_frame_to_flow_data(mt_module_http_t *module_http,
                                                 mt_module_flow_t *flow_in) {
  char *data_out = NULL;
  cJSON *root = NULL;
  cJSON *config_in_json = NULL;
  cJSON *flow_in_json = NULL;

  // check argument
  if (module_http == NULL) {
    ESP_LOGE(TAG, "%4d %s module_http is NULL", __LINE__, __func__);
    goto EXIT;
  } else {
    if (module_http->module == NULL) {
      ESP_LOGE(TAG, "%4d %s module_http->module is NULL", __LINE__, __func__);
      goto EXIT;
    } else {
      if (module_http->module->id == NULL) {
        ESP_LOGE(TAG, "%4d %s module_http->module->id is NULL", __LINE__,
                 __func__);
        goto EXIT;
      }
    }
    if (flow_in == NULL) {
      ESP_LOGE(TAG, "%4d %s flow_in is NULL", __LINE__, __func__);
      goto EXIT;
    } else {
      if (flow_in->flow == NULL) {
        ESP_LOGE(TAG, "%4d %s flow_in->flow  is NULL", __LINE__, __func__);
        goto EXIT;
      } else {
        if (flow_in->flow->name == NULL) {
          ESP_LOGE(TAG, "%4d %s flow_in->flow->name is NULL", __LINE__,
                   __func__);
          goto EXIT;
        }
      }
    }
  }

  // request post_data
  root = cJSON_CreateObject();
  cJSON_AddStringToObject(root, "id", module_http->module->id);
  cJSON_AddItemToObject(root, "config", config_in_json = cJSON_CreateObject());
  cJSON_AddItemToObject(config_in_json, "flow",
                        flow_in_json = cJSON_CreateObject());
  cJSON_AddStringToObject(flow_in_json, "name", flow_in->flow->name);
  cJSON_AddBoolToObject(config_in_json, "config_ack", flow_in->config_ack);
  cJSON_AddBoolToObject(config_in_json, "push_ack", flow_in->push_ack);
  data_out = cJSON_Print(root);
  cJSON_Delete(root);

  //  ESP_LOGI(TAG, "%4d %s data_out =%s", __LINE__, __func__, data_out);

EXIT:
  return data_out;
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

  err = mt_mqtt_pub_msg(ping_topic, frame_buf, frame_size);
  free(frame_buf);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_mqtt_pub_msg failed", __LINE__, __func__);
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
  module_flow->flow_handle = NULL;
  module_flow->create_push_frame_interval = 30 * 1000; // 30s
  module_flow->push_frame_interval = 20 * 1000;        // 20s
  module_flow->ping_interval = 30 * 1000;              // 30s
  module_flow->ping_retry_times = 3; // retry 3 times, nor to restart
  module_flow->ping_count = 0;
  module_flow->push_ack = true;
  module_flow->config_ack = true;
  module_flow->poll_enable = false;
  module_flow->poll_interval = 0;
  module_flow->poll_handle = ESP_OK;
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
  uint8_t frame_num = 0;
  uint8_t frame_count = 0;

  if (value_in->size <= 0) {
    ESP_LOGE(TAG, "%4d %s value_in->size:%d error", __LINE__, __func__,
             value_in->size);
    return NULL;
  }

  // frame size which key not NULL
  for (int i = 0; i < value_in->size; i++) {
    if (value_in->value[i]->key != NULL) {
      frame_num++;
    }
  }

  if (frame_num == 0) {
    ESP_LOGE(TAG, "%4d %s frame_num zero", __LINE__, __func__);
    return NULL;
  }

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

  for (int i = 0; i < value_in->size; i++) {
    // ignore NULL key
    if (value_in->value[i]->key == NULL) {
      continue;
    }
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

uint8_t *mt_module_flow_repack_frame_with_session(uint8_t *buf_in, int size_in,
                                                  char *session_id,
                                                  int *size_out) {
  uint8_t *buf_out = NULL;
  PushFrameToFlowRequest *frame_req = NULL;

  // arg check
  if (buf_in == NULL) {
    ESP_LOGE(TAG, "%4d %s buf_in NULL", __LINE__, __func__);
    return NULL;
  }
  if (session_id == NULL) {
    ESP_LOGE(TAG, "%4d %s session_id NULL", __LINE__, __func__);
    return NULL;
  }

  // unpack
  frame_req = push_frame_to_flow_request__unpack(NULL, size_in, buf_in);
  if (frame_req == NULL) {
    ESP_LOGE(TAG, "%4d %s push_frame_to_flow_request__unpack failed", __LINE__,
             __func__);
    return NULL;
  }

  google__protobuf__string_value__init(frame_req->id);
  int session_size = strlen(session_id) + 1;
  frame_req->id->value = malloc(session_size);
  memcpy(frame_req->id->value, session_id, session_size);

  // pack frame
  *size_out = push_frame_to_flow_request__get_packed_size(frame_req);
  buf_out = malloc(*size_out);
  push_frame_to_flow_request__pack(frame_req, buf_out);

  // free
  push_frame_to_flow_request__free_unpacked(frame_req, NULL);

  return buf_out;
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
          continue;
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
  err = mt_mqtt_pub_msg(req_topic, frame_req_buf, frame_req_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_mqtt_pub_msg failed", __LINE__, __func__);
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

esp_err_t
mt_module_flow_sent_msg_check_default(mt_module_flow_t *module_flow,
                                      mt_module_flow_struct_group_t *msg) {
  mt_module_flow_struct_group_t *msg_check_default = NULL;
  uint32_t new_size = 0;
  uint32_t count = 0;

  // get size
  for (int i = 0; i < msg->size; i++) {
    if (msg->value[i]->use_default == false)
      new_size++;

    switch (msg->value[i]->type) {
    case GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE:
      if (msg->value[i]->bool_value != msg->value[i]->default_bool_value) {
        new_size++;
      }
      break;
    case GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE:
      if (msg->value[i]->number_value != msg->value[i]->default_double_value) {
        new_size++;
      }
      break;
    case GOOGLE__PROTOBUF__VALUE__KIND_STRING_VALUE:
      if (strcmp(msg->value[i]->string_value,
                 msg->value[i]->default_string_value) != 0) {
        new_size++;
      }
      break;
    default:
      ESP_LOGE(TAG, "%4d %s unexcept type:%d ", __LINE__, __func__,
               msg->value[i]->type);
      goto EXIT;
      break;
    }
  }

  // check size
  if (new_size <= 0)
    goto EXIT;

  // copy not default
  msg_check_default = mt_module_flow_new_struct_group(new_size);
  for (int i = 0; i < msg->size; i++) {
    bool diff = false;
    if (msg->value[i]->use_default == false)
      diff = true;
    switch (msg->value[i]->type) {
    case GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE:
      if (msg->value[i]->bool_value != msg->value[i]->default_bool_value) {
        diff = true;
      }
      break;
    case GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE:
      if (msg->value[i]->number_value != msg->value[i]->default_double_value) {
        diff = true;
      }
      break;
    case GOOGLE__PROTOBUF__VALUE__KIND_STRING_VALUE:
      if (strcmp(msg->value[i]->string_value,
                 msg->value[i]->default_string_value) != 0) {
        diff = true;
      }
      break;
    default:
      ESP_LOGE(TAG, "%4d %s unexcept type:%d ", __LINE__, __func__,
               msg->value[i]->type);
      goto EXIT;
      break;
    }
    if (diff == true) {
      mt_module_flow_copy_struct(msg->value[i],
                                 msg_check_default->value[count]);
      count++;
    }
  }

  // sent msg
  mt_module_flow_sent_msg(module_flow, msg_check_default);

EXIT:
  if (msg_check_default != NULL)
    mt_module_flow_free_struct_group(msg_check_default);
  return ESP_OK;
}
