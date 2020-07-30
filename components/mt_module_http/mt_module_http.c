#include "stdlib.h"
#include "string.h"

#include "cJSON.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "mt_http_client.h"
#include "mt_module_http.h"
#include "mt_module_http_manage.h"
#include "mt_module_http_utils.h"
#include "mt_mqtt.h"
#include "mt_mqtt_lan.h"
#include "mt_nvs_config.h"
#include "mt_utils.h"
#include "mt_utils_login.h"
#include "mt_utils_session.h"

// global define ==============================================================

static const char *TAG = "MT_MODULE_HTTP";

#define MAX_HTTP_RECV_BUFFER 512
mt_module_http_t *MODULE_HTTP = NULL;
extern uint64_t Session_id;

// static func ================================================================

static esp_err_t mt_module_save_content(char *content, int size) {
  if (MODULE_HTTP->response_content != NULL) {
    free(MODULE_HTTP->response_content);
    MODULE_HTTP->response_content = NULL;
  }

  if (size <= 0) {
    ESP_LOGE(TAG, "%4d %s content size =%d", __LINE__, __func__, size);
    return ESP_ERR_INVALID_ARG;
  }

  MODULE_HTTP->response_content = malloc(size + 1);

  memcpy(MODULE_HTTP->response_content, content, size);
  MODULE_HTTP->response_content[size] = '\0';
  MODULE_HTTP->response_content_size = size;

  ESP_LOGI(TAG, "%4d save content, size=%d, string=%s", __LINE__, size,
           MODULE_HTTP->response_content);

  return ESP_OK;
}

static esp_err_t module_http_event_handler(esp_http_client_event_t *evt) {
  switch (evt->event_id) {
  case HTTP_EVENT_ERROR:
    ESP_LOGI(TAG, "%4d HTTP_EVENT_EXIT", __LINE__);
    break;
  case HTTP_EVENT_ON_CONNECTED:
    ESP_LOGD(TAG, "%4d HTTP_EVENT_ON_CONNECTED", __LINE__);
    break;
  case HTTP_EVENT_HEADER_SENT:
    ESP_LOGD(TAG, "%4d HTTP_EVENT_HEADER_SENT", __LINE__);
    break;
  case HTTP_EVENT_ON_HEADER:
    ESP_LOGD(TAG, "%4d HTTP_EVENT_ON_HEADER, key=%s, value=%s", __LINE__,
             evt->header_key, evt->header_value);
    break;
  case HTTP_EVENT_ON_DATA:
    if (!esp_http_client_is_chunked_response(evt->client)) {
      esp_err_t err;

      ESP_LOGD(TAG, "%4d HTTP_EVENT_ON_DATA, len=%d, data=%s", __LINE__,
               evt->data_len, (char *)evt->data);
      err = mt_module_save_content((char *)evt->data, evt->data_len);
      if (err != ESP_OK) {
        ESP_LOGE(TAG, "%4d mt_module_save_content failed", __LINE__);
        break;
      }
    }
    break;
  case HTTP_EVENT_ON_FINISH:
    ESP_LOGD(TAG, "%4d HTTP_EVENT_ON_FINISH", __LINE__);
    break;
  case HTTP_EVENT_DISCONNECTED:
    ESP_LOGD(TAG, "%4d HTTP_EVENT_DISCONNECTED", __LINE__);
    break;
  }
  return ESP_OK;
}

// global func ================================================================

esp_err_t
mt_module_http_actions_issue_module_token(mt_module_http_t *module_http) {
  esp_err_t err = ESP_OK;
  char *post_data = NULL;
  esp_http_client_handle_t client = NULL;
  token_t *tkn_out = NULL;

  esp_http_client_config_t config = {
      .host = module_http->host,
      .port = module_http->port,
      .transport_type = module_http->tran_type,
      .path = "/v1/device_cloud/actions/issue_module_token",
      .event_handler = module_http->event_handler,
  };

  ESP_LOGI(TAG, "%4d %s mt_module_http_actions_issue_module_token", __LINE__,
           __func__);

  client = esp_http_client_init(&config);

  // post data
  post_data = mt_utils_login_get_issue_token_data(module_http);
  if (post_data == NULL) {
    ESP_LOGE(TAG, "%4d %s post_data NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // request
  err = mt_http_client_post_request(client, NULL, post_data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 201) {
    ESP_LOGI(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // parse res content
  int content_size = esp_http_client_get_content_length(client);
  if (content_size != 0) {
    if (content_size != module_http->response_content_size) {
      ESP_LOGE(TAG,
               "%4d content_size %d != module_http->response_content_size %d",
               __LINE__, content_size, module_http->response_content_size);
      err = ESP_ERR_HTTP_BASE;
      goto EXIT;
    } else {
      tkn_out =
          mt_module_http_utils_parse_token_res(module_http->response_content);
      if (tkn_out == NULL) {
        ESP_LOGE(TAG, "%4d mt_module_http_utils_parse_token_res failed code=%d",
                 __LINE__, err);
        err = ESP_ERR_HTTP_BASE;
        goto EXIT;
      }

      if (tkn_out == NULL) {
        ESP_LOGE(TAG, "%4d %s token NULL", __LINE__, __func__);
        err = ESP_ERR_HTTP_BASE;
        goto EXIT;
      } else {
        if (tkn_out->text == NULL) {
          ESP_LOGE(TAG, "%4d %s token->text NULL", __LINE__, __func__);
          err = ESP_ERR_HTTP_BASE;
          goto EXIT;
        }
      }
    }
  }

  ESP_LOGI(TAG, "%4d %s request ok", __LINE__, __func__);
  if (tkn_out != NULL) {
    if (tkn_out->text != NULL) {
      // ESP_LOGW(TAG, "%4d %s token=%s", __LINE__, __func__, tkn_out->text);
    }
  }

  if (module_http->token != NULL) {
    free(module_http->token);
  }

  module_http->token = malloc(strlen(tkn_out->text) + 1);
  module_http->token[strlen(tkn_out->text)] = '\0';
  memcpy(module_http->token, tkn_out->text, strlen(tkn_out->text));

EXIT:
  // clean
  esp_http_client_cleanup(client);

  if (post_data != NULL)
    free(post_data);

  if (tkn_out != NULL)
    mt_module_http_utils_free_token(tkn_out);

  if (module_http->response_content != NULL) {
    free(module_http->response_content);
    module_http->response_content = NULL;
  }

  return err;
}

module_t *mt_module_http_actions_show_module(mt_module_http_t *module_http) {
  esp_err_t err;
  esp_http_client_handle_t client = NULL;
  module_t *mdl_out = NULL;
  esp_http_client_config_t config = {
      .host = module_http->host,
      .port = module_http->port,
      .transport_type = module_http->tran_type,
      .path = "/v1/device_cloud/actions/show_module",
      .event_handler = module_http->event_handler,
  };

  ESP_LOGI(TAG, "%4d %s mt_module_http_actions_show_module", __LINE__,
           __func__);

  client = esp_http_client_init(&config);

  // check argument
  if (module_http->token == NULL) {
    ESP_LOGE(TAG, "%4d module_http->token is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // request
  err = mt_http_client_post_request(client, module_http->token, NULL);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 200) {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    err = ESP_ERR_HTTP_BASE;
    goto EXIT;
  }

  // parse res content
  int content_size = esp_http_client_get_content_length(client);
  if (content_size != 0) {
    if (content_size != module_http->response_content_size) {
      ESP_LOGE(TAG, "%4d content_size %d != response_content_size %d", __LINE__,
               content_size, module_http->response_content_size);
      err = ESP_ERR_HTTP_BASE;
      goto EXIT;
    } else {
      mdl_out =
          mt_module_http_uitls_parse_module_res(module_http->response_content);
      if (mdl_out == NULL) {
        ESP_LOGE(TAG, "%4d mt_module_http_utils_parse_token_res failed code=%d",
                 __LINE__, err);
        err = ESP_ERR_HTTP_BASE;
        goto EXIT;
      } else {
        if (mdl_out->id == NULL) {
          ESP_LOGE(TAG,
                   "%4d mt_module_http_utils_parse_token_res module_id NULL",
                   __LINE__);
          err = ESP_ERR_HTTP_BASE;
          goto EXIT;
        }
      }
    }
  }

  ESP_LOGI(TAG, "%4d %s request ok:id=%s", __LINE__, __func__, mdl_out->id);

EXIT:
  // clean
  esp_http_client_cleanup(client);

  if (module_http->response_content != NULL) {
    free(module_http->response_content);
    module_http->response_content = NULL;
  }

  if (err != ESP_OK) {
    if (mdl_out != NULL) {
      mt_module_http_utils_free_module(mdl_out);
    }
    return NULL;
  }

  return mdl_out;
}

esp_err_t mt_module_http_actions_heartbeat(mt_module_http_t *module_http,
                                           module_t *mod_in) {
  esp_err_t err = ESP_OK;
  char *post_data = NULL;
  esp_http_client_handle_t client = NULL;
  cJSON *root = NULL;
  uint8_t str_size = 0;
  char *session_id_str = NULL;

  ESP_LOGI(TAG, "%4d %s mt_module_http_actions_heartbeat", __LINE__, __func__);

  esp_http_client_config_t config = {
      .host = module_http->host,
      .port = module_http->port,
      .transport_type = module_http->tran_type,
      .path = "/v1/device_cloud/actions/heartbeat",
      .event_handler = module_http->event_handler,
  };

  client = esp_http_client_init(&config);

  // check argument
  if (module_http->token == NULL) {
    ESP_LOGE(TAG, "%4d token is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (module_http->session_id == 0) {
    ESP_LOGE(TAG, "%4d session_id is 0", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (mod_in == NULL) {
    ESP_LOGE(TAG, "%4d %s mod_in is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  } else {
    if (mod_in->name == NULL) {
      ESP_LOGE(TAG, "%4d %s mod_in->name is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  // request post_data
  root = cJSON_CreateObject();
  post_data = cJSON_Print(root);
  cJSON_Delete(root);

  // request extra header
  session_id_str = mt_utils_int64_to_string(module_http->session_id);
  err = esp_http_client_set_header(client, "MT-Module-Session", session_id_str);
  if (err != ESP_OK) {
    ESP_LOGE(TAG,
             "%4d esp_http_client_set_header MT-Module-Session failed code=%d",
             __LINE__, err);
    goto EXIT;
  }

  // request
  err = mt_http_client_post_request(client, module_http->token, post_data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 204) {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    err = ESP_ERR_HTTP_BASE;
    goto EXIT;
  }

  ESP_LOGI(TAG, "%4d %s request ok", __LINE__, __func__);

EXIT:
  // clean
  if (post_data != NULL) {
    free(post_data);
  }

  if (session_id_str != NULL) {
    free(session_id_str);
  }

  esp_http_client_cleanup(client);

  return err;
}

esp_err_t mt_module_http_actions_put_object(mt_module_http_t *module_http,
                                            object_t *obj_in,
                                            char *content_in) {
  esp_err_t err = ESP_OK;
  char *post_data;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *obj_in_json, *obj_in_device_json;

  esp_http_client_config_t config = {
      .host = module_http->host,
      .port = module_http->port,
      .transport_type = module_http->tran_type,
      .path = "/v1/device_cloud/actions/put_object",
      .event_handler = module_http->event_handler,
  };

  ESP_LOGI(TAG, "%4d %s mt_module_http_actions_put_object", __LINE__, __func__);

  client = esp_http_client_init(&config);

  // check argument
  if (module_http->token == NULL) {
    ESP_LOGE(TAG, "%4d token is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (obj_in == NULL) {
    ESP_LOGE(TAG, "%4d %s obj_in is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  } else {
    if (obj_in->device == NULL) {
      ESP_LOGE(TAG, "%4d %s obj_in->device is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    } else {
      if (obj_in->device->id == NULL) {
        ESP_LOGE(TAG, "%4d %s obj_in->device->id is NULL", __LINE__, __func__);
        err = ESP_ERR_INVALID_ARG;
        goto EXIT;
      }
    }

    if (obj_in->prefix == NULL) {
      ESP_LOGE(TAG, "%4d %s obj_in->prefix is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }

    if (obj_in->name == NULL) {
      ESP_LOGE(TAG, "%4d %s obj_in->name is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  if (content_in == NULL) {
    ESP_LOGE(TAG, "%4d content_in is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // request post_data
  root = cJSON_CreateObject();
  cJSON_AddItemToObject(root, "object", obj_in_json = cJSON_CreateObject());
  cJSON_AddItemToObject(obj_in_json, "device",
                        obj_in_device_json = cJSON_CreateObject());
  cJSON_AddStringToObject(obj_in_device_json, "id", obj_in->device->id);
  cJSON_AddStringToObject(obj_in_json, "prefix", obj_in->prefix);
  cJSON_AddStringToObject(obj_in_json, "name", obj_in->name);
  cJSON_AddStringToObject(root, "content", content_in);
  post_data = cJSON_Print(root);
  cJSON_Delete(root);

  ESP_LOGI(TAG, "%4d %s post_data =%s", __LINE__, __func__, post_data);

  // requset
  err = mt_http_client_post_request(client, module_http->token, post_data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 204) {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    err = ESP_ERR_HTTP_BASE;
    goto EXIT;
  }

  ESP_LOGI(TAG, "%4d %s request ok", __LINE__, __func__);

EXIT:
  // clean
  esp_http_client_cleanup(client);

  return err;
}

esp_err_t mt_module_http_actions_remove_object(mt_module_http_t *module_http,
                                               object_t *obj_in) {
  esp_err_t err = ESP_OK;
  char *post_data;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *obj_in_json, *obj_in_device_json;

  esp_http_client_config_t config = {
      .host = module_http->host,
      .port = module_http->port,
      .transport_type = module_http->tran_type,
      .path = "/v1/device_cloud/actions/remove_object",
      .event_handler = module_http->event_handler,
  };

  ESP_LOGI(TAG, "%4d %s mt_module_http_actions_remove_object", __LINE__,
           __func__);

  client = esp_http_client_init(&config);

  // check argument
  if (module_http->token == NULL) {
    ESP_LOGE(TAG, "%4d token is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (obj_in == NULL) {
    ESP_LOGE(TAG, "%4d %s obj_in is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  } else {
    if (obj_in->device == NULL) {
      ESP_LOGE(TAG, "%4d %s obj_in->device is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    } else {
      if (obj_in->device->id == NULL) {
        ESP_LOGE(TAG, "%4d %s obj_in->device->id is NULL", __LINE__, __func__);
        err = ESP_ERR_INVALID_ARG;
        goto EXIT;
      }
    }

    if (obj_in->prefix == NULL) {
      ESP_LOGE(TAG, "%4d %s obj_in->prefix is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }

    if (obj_in->name == NULL) {
      ESP_LOGE(TAG, "%4d %s obj_in->name is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  // request post_data
  root = cJSON_CreateObject();
  cJSON_AddItemToObject(root, "object", obj_in_json = cJSON_CreateObject());
  cJSON_AddItemToObject(obj_in_json, "device",
                        obj_in_device_json = cJSON_CreateObject());
  cJSON_AddStringToObject(obj_in_device_json, "id", obj_in->device->id);
  cJSON_AddStringToObject(obj_in_json, "prefix", obj_in->prefix);
  cJSON_AddStringToObject(obj_in_json, "name", obj_in->name);
  post_data = cJSON_Print(root);
  cJSON_Delete(root);

  ESP_LOGI(TAG, "%4d %s post_data =%s", __LINE__, __func__, post_data);

  // requset
  err = mt_http_client_post_request(client, module_http->token, post_data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 204) {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    err = ESP_ERR_HTTP_BASE;
    goto EXIT;
  }

  ESP_LOGI(TAG, "%4d %s request ok", __LINE__, __func__);

EXIT:
  // clean
  esp_http_client_cleanup(client);

  return err;
}

esp_err_t mt_module_http_actions_rename_object(mt_module_http_t *module_http,
                                               object_t *src_in,
                                               object_t *des_in) {
  esp_err_t err = ESP_OK;
  char *post_data;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *src_in_json, *src_in_device_json, *des_in_json,
      *des_in_device_json;

  esp_http_client_config_t config = {
      .host = module_http->host,
      .port = module_http->port,
      .transport_type = module_http->tran_type,
      .path = "/v1/device_cloud/actions/rename_object",
      .event_handler = module_http->event_handler,
  };

  ESP_LOGI(TAG, "%4d %s mt_module_http_actions_rename_object", __LINE__,
           __func__);

  client = esp_http_client_init(&config);

  // check argument
  if (module_http->token == NULL) {
    ESP_LOGE(TAG, "%4d token is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (src_in == NULL) {
    ESP_LOGE(TAG, "%4d %s src_in is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  } else {
    if (src_in->device == NULL) {
      ESP_LOGE(TAG, "%4d %s src_in->device is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    } else {
      if (src_in->device->id == NULL) {
        ESP_LOGE(TAG, "%4d %s src_in->device->id is NULL", __LINE__, __func__);
        err = ESP_ERR_INVALID_ARG;
        goto EXIT;
      }
    }

    if (src_in->prefix == NULL) {
      ESP_LOGE(TAG, "%4d %s src_in->prefix is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }

    if (src_in->name == NULL) {
      ESP_LOGE(TAG, "%4d %s src_in->name is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  if (des_in == NULL) {
    ESP_LOGE(TAG, "%4d %s des_in is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  } else {
    if (des_in->device == NULL) {
      ESP_LOGE(TAG, "%4d %s des_in->device is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    } else {
      if (des_in->device->id == NULL) {
        ESP_LOGE(TAG, "%4d %s des_in->device->id is NULL", __LINE__, __func__);
        err = ESP_ERR_INVALID_ARG;
        goto EXIT;
      }
    }

    if (des_in->prefix == NULL) {
      ESP_LOGE(TAG, "%4d %s des_in->prefix is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }

    if (des_in->name == NULL) {
      ESP_LOGE(TAG, "%4d %s des_in->name is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  // request post_data
  root = cJSON_CreateObject();
  cJSON_AddItemToObject(root, "source", src_in_json = cJSON_CreateObject());
  cJSON_AddItemToObject(src_in_json, "device",
                        src_in_device_json = cJSON_CreateObject());
  cJSON_AddStringToObject(src_in_device_json, "id", src_in->device->id);
  cJSON_AddStringToObject(src_in_json, "prefix", src_in->prefix);
  cJSON_AddStringToObject(src_in_json, "name", src_in->name);
  cJSON_AddItemToObject(root, "destination",
                        des_in_json = cJSON_CreateObject());
  cJSON_AddItemToObject(des_in_json, "device",
                        des_in_device_json = cJSON_CreateObject());
  cJSON_AddStringToObject(des_in_device_json, "id", des_in->device->id);
  cJSON_AddStringToObject(des_in_json, "prefix", des_in->prefix);
  cJSON_AddStringToObject(des_in_json, "name", des_in->name);
  post_data = cJSON_Print(root);
  cJSON_Delete(root);

  ESP_LOGI(TAG, "%4d %s post_data =%s", __LINE__, __func__, post_data);

  // requset
  err = mt_http_client_post_request(client, module_http->token, post_data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 204) {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    err = ESP_ERR_HTTP_BASE;
    goto EXIT;
  }

  ESP_LOGI(TAG, "%4d %s request ok", __LINE__, __func__);

EXIT:
  // clean
  esp_http_client_cleanup(client);

  return err;
}

object_t *mt_module_http_actions_get_object(mt_module_http_t *module_http,
                                            object_t *obj_in) {
  esp_err_t err = ESP_OK;
  object_t *obj_out = NULL;
  char *post_data;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *obj_in_json, *obj_in_device_json;

  esp_http_client_config_t config = {
      .host = module_http->host,
      .port = module_http->port,
      .transport_type = module_http->tran_type,
      .path = "/v1/device_cloud/actions/get_object",
      .event_handler = module_http->event_handler,
  };

  ESP_LOGI(TAG, "%4d %s mt_module_http_actions_get_object", __LINE__, __func__);

  client = esp_http_client_init(&config);

  // check argument
  if (module_http->token == NULL) {
    ESP_LOGE(TAG, "%4d token is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (obj_in == NULL) {
    ESP_LOGE(TAG, "%4d %s obj_in is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  } else {
    if (obj_in->device == NULL) {
      ESP_LOGE(TAG, "%4d %s obj_in->device is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    } else {
      if (obj_in->device->id == NULL) {
        ESP_LOGE(TAG, "%4d %s obj_in->device->id is NULL", __LINE__, __func__);
        err = ESP_ERR_INVALID_ARG;
        goto EXIT;
      }
    }

    if (obj_in->prefix == NULL) {
      ESP_LOGE(TAG, "%4d %s obj_in->prefix is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }

    if (obj_in->name == NULL) {
      ESP_LOGE(TAG, "%4d %s obj_in->name is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  // request post_data
  root = cJSON_CreateObject();
  cJSON_AddItemToObject(root, "object", obj_in_json = cJSON_CreateObject());
  cJSON_AddItemToObject(obj_in_json, "device",
                        obj_in_device_json = cJSON_CreateObject());
  cJSON_AddStringToObject(obj_in_device_json, "id", obj_in->device->id);
  cJSON_AddStringToObject(obj_in_json, "prefix", obj_in->prefix);
  cJSON_AddStringToObject(obj_in_json, "name", obj_in->name);
  post_data = cJSON_Print(root);
  cJSON_Delete(root);

  ESP_LOGI(TAG, "%4d %s post_data =%s", __LINE__, __func__, post_data);

  // requset
  err = mt_http_client_post_request(client, module_http->token, post_data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 200) {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    err = ESP_ERR_HTTP_BASE;
    goto EXIT;
  }

  // parse res content
  int content_size = esp_http_client_get_content_length(client);
  if (content_size != 0) {
    if (content_size != module_http->response_content_size) {
      ESP_LOGE(TAG, "%4d content_size %d != response_content_size %d", __LINE__,
               content_size, module_http->response_content_size);
      err = ESP_ERR_HTTP_BASE;
      goto EXIT;
    } else {
      obj_out =
          mt_module_http_utils_parse_object_res(module_http->response_content);
      if (obj_out == NULL) {
        ESP_LOGE(TAG,
                 "%4d mt_module_http_utils_parse_object_res failed code=%d",
                 __LINE__, err);
        err = ESP_ERR_HTTP_BASE;
        goto EXIT;
      }
    }
  }

  ESP_LOGI(TAG, "%4d %s request ok", __LINE__, __func__);

EXIT:
  // clean
  esp_http_client_cleanup(client);

  if (err != ESP_OK) {
    if (obj_out != NULL) {
      mt_module_http_utils_free_object(obj_out);
    }

    return NULL;
  }

  return obj_out;
}

char *mt_module_http_actions_get_object_content(mt_module_http_t *module_http,
                                                object_t *obj_in) {
  esp_err_t err = ESP_OK;
  char *content_out = NULL;
  char *post_data;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *obj_in_json, *obj_in_device_json;

  esp_http_client_config_t config = {
      .host = module_http->host,
      .port = module_http->port,
      .transport_type = module_http->tran_type,
      .path = "/v1/device_cloud/actions/get_object_content",
      .event_handler = module_http->event_handler,
  };

  ESP_LOGI(TAG, "%4d %s mt_module_http_actions_get_object_content", __LINE__,
           __func__);

  client = esp_http_client_init(&config);

  // check argument
  if (module_http->token == NULL) {
    ESP_LOGE(TAG, "%4d token is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (obj_in == NULL) {
    ESP_LOGE(TAG, "%4d %s obj_in is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  } else {
    if (obj_in->device == NULL) {
      ESP_LOGE(TAG, "%4d %s obj_in->device is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    } else {
      if (obj_in->device->id == NULL) {
        ESP_LOGE(TAG, "%4d %s obj_in->device->id is NULL", __LINE__, __func__);
        err = ESP_ERR_INVALID_ARG;
        goto EXIT;
      }
    }

    if (obj_in->prefix == NULL) {
      ESP_LOGE(TAG, "%4d %s obj_in->prefix is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }

    if (obj_in->name == NULL) {
      ESP_LOGE(TAG, "%4d %s obj_in->name is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  // request post_data
  root = cJSON_CreateObject();
  cJSON_AddItemToObject(root, "object", obj_in_json = cJSON_CreateObject());
  cJSON_AddItemToObject(obj_in_json, "device",
                        obj_in_device_json = cJSON_CreateObject());
  cJSON_AddStringToObject(obj_in_device_json, "id", obj_in->device->id);
  cJSON_AddStringToObject(obj_in_json, "prefix", obj_in->prefix);
  cJSON_AddStringToObject(obj_in_json, "name", obj_in->name);
  post_data = cJSON_Print(root);
  cJSON_Delete(root);

  ESP_LOGI(TAG, "%4d %s post_data =%s", __LINE__, __func__, post_data);

  // requset
  err = mt_http_client_post_request(client, module_http->token, post_data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 200) {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    err = ESP_ERR_HTTP_BASE;
    goto EXIT;
  }

  // parse res content
  int content_size = esp_http_client_get_content_length(client);
  if (content_size != 0) {
    if (content_size != module_http->response_content_size) {
      ESP_LOGE(TAG, "%4d content_size %d != response_content_size %d", __LINE__,
               content_size, module_http->response_content_size);
      err = ESP_ERR_HTTP_BASE;
      goto EXIT;
    } else {
      content_out =
          mt_module_http_utils_parse_content_res(module_http->response_content);
      if (content_out == NULL) {
        ESP_LOGE(TAG,
                 "%4d mt_module_http_utils_parse_content_res failed code=%d",
                 __LINE__, err);
        err = ESP_ERR_HTTP_BASE;
        goto EXIT;
      }
    }
  }

  ESP_LOGI(TAG, "%4d %s request ok", __LINE__, __func__);

EXIT:
  // clean
  esp_http_client_cleanup(client);

  if (err != ESP_OK) {
    if (content_out != NULL)
      free(content_out);

    return NULL;
  }

  return content_out;
}

uint8_t *mt_module_http_actions_list_objects(mt_module_http_t *module_http,
                                             object_t *obj_in, int *objs_num) {
  esp_err_t err = ESP_OK;
  uint8_t *objs_out = NULL;
  char *post_data;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *obj_in_json, *obj_in_device_json;

  esp_http_client_config_t config = {
      .host = module_http->host,
      .port = module_http->port,
      .transport_type = module_http->tran_type,
      .path = "/v1/device_cloud/actions/list_objects",
      .event_handler = module_http->event_handler,
  };

  ESP_LOGI(TAG, "%4d %s mt_module_http_actions_list_objects", __LINE__,
           __func__);

  client = esp_http_client_init(&config);

  // check argument
  if (module_http->token == NULL) {
    ESP_LOGE(TAG, "%4d token is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (obj_in == NULL) {
    ESP_LOGE(TAG, "%4d %s obj_in is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // request post_data
  root = cJSON_CreateObject();
  cJSON_AddItemToObject(root, "object", obj_in_json = cJSON_CreateObject());
  if (obj_in->device != NULL) {
    cJSON_AddItemToObject(obj_in_json, "device",
                          obj_in_device_json = cJSON_CreateObject());
    if (obj_in->device->id != NULL) {
      cJSON_AddStringToObject(obj_in_device_json, "id", obj_in->device->id);
    }
  }
  if (obj_in->prefix != NULL) {
    cJSON_AddStringToObject(obj_in_json, "prefix", obj_in->prefix);
  }
  if (obj_in->name != NULL) {
    cJSON_AddStringToObject(obj_in_json, "name", obj_in->name);
  }
  post_data = cJSON_Print(root);
  cJSON_Delete(root);

  ESP_LOGI(TAG, "%4d %s post_data =%s", __LINE__, __func__, post_data);

  // requset
  err = mt_http_client_post_request(client, module_http->token, post_data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 204) {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    err = ESP_ERR_HTTP_BASE;
    goto EXIT;
  }

  // parse res content
  int content_size = esp_http_client_get_content_length(client);
  if (content_size != 0) {
    if (content_size != module_http->response_content_size) {
      ESP_LOGE(TAG, "%4d content_size %d != response_content_size %d", __LINE__,
               content_size, module_http->response_content_size);
      err = ESP_ERR_HTTP_BASE;
      goto EXIT;
    } else {
      objs_out = mt_module_http_utils_parse_objects_res(
          module_http->response_content, objs_num);
      if (objs_out == NULL) {
        ESP_LOGE(TAG,
                 "%4d mt_module_http_utils_parse_objects_res failed code=%d",
                 __LINE__, err);
        err = ESP_ERR_HTTP_BASE;
        goto EXIT;
      }
    }
  }

  ESP_LOGI(TAG, "%4d %s request ok", __LINE__, __func__);

EXIT:
  // clean
  esp_http_client_cleanup(client);

  if (err != ESP_OK) {
    if (objs_out != NULL) {
      free(objs_out);
    }
    return NULL;
  }

  return objs_out;
}

push_frame_res_t *
mt_module_http_actions_push_frame_to_flow(mt_module_http_t *module_http,
                                          flow_t *flow_in, bool config_ack_in,
                                          bool push_ack_in) {
  esp_err_t err = ESP_OK;
  char *post_data = NULL;
  esp_http_client_handle_t client = NULL;
  cJSON *root = NULL;
  cJSON *config_in_json = NULL;
  cJSON *flow_in_json = NULL;
  push_frame_res_t *res_out = NULL;

  esp_http_client_config_t config = {
      .host = module_http->host,
      .port = module_http->port,
      .transport_type = module_http->tran_type,
      .path = "/v1/device_cloud/actions/push_frame_to_flow",
      .event_handler = module_http->event_handler,
  };

  ESP_LOGI(TAG, "%4d %s mt_module_http_actions_push_frame_to_flow", __LINE__,
           __func__);

  client = esp_http_client_init(&config);

  // check argument
  if (module_http->token == NULL) {
    ESP_LOGE(TAG, "%4d token is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (module_http->module == NULL) {
    ESP_LOGE(TAG, "%4d %s module_http->module is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  } else {
    if (module_http->module->id == NULL) {
      ESP_LOGE(TAG, "%4d %s module_http->module->id is NULL", __LINE__,
               __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  if (flow_in == NULL) {
    ESP_LOGE(TAG, "%4d %s flow_in is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  } else {
    if (flow_in->name == NULL) {
      ESP_LOGE(TAG, "%4d %s flow_in->name is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  // request post_data
  root = cJSON_CreateObject();
  cJSON_AddStringToObject(root, "id", module_http->module->id);
  cJSON_AddItemToObject(root, "config", config_in_json = cJSON_CreateObject());
  cJSON_AddItemToObject(config_in_json, "flow",
                        flow_in_json = cJSON_CreateObject());
  cJSON_AddStringToObject(flow_in_json, "name", flow_in->name);
  cJSON_AddBoolToObject(config_in_json, "config_ack", config_ack_in);
  cJSON_AddBoolToObject(config_in_json, "push_ack", push_ack_in);
  post_data = cJSON_Print(root);
  cJSON_Delete(root);

  ESP_LOGI(TAG, "%4d %s post_data =%s", __LINE__, __func__, post_data);

  // request
  err = mt_http_client_post_request(client, module_http->token, post_data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 200) {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    err = ESP_ERR_HTTP_BASE;
    goto EXIT;
  }

  // parse res content
  int content_size = esp_http_client_get_content_length(client);
  if (content_size != 0) {
    if (content_size != module_http->response_content_size) {
      ESP_LOGE(TAG,
               "%4d content_size %d != module_http->response_content_size %d",
               __LINE__, content_size, module_http->response_content_size);
      err = ESP_ERR_HTTP_BASE;
      goto EXIT;
    } else {
      res_out = mt_module_http_utils_parse_push_frame_res(
          module_http->response_content);
      if (res_out == NULL) {
        ESP_LOGE(TAG, "%4d mt_module_http_utils_parse_token_res failed code=%d",
                 __LINE__, err);
        err = ESP_ERR_HTTP_BASE;
        goto EXIT;
      }

      if (res_out == NULL) {
        ESP_LOGE(TAG, "%4d %s token NULL", __LINE__, __func__);
        err = ESP_ERR_HTTP_BASE;
        goto EXIT;
      } else {
        if (res_out->session_id == NULL) {
          ESP_LOGE(TAG, "%4d %s res_out->session_id NULL", __LINE__, __func__);
          err = ESP_ERR_HTTP_BASE;
          goto EXIT;
        }
      }
    }
  }

  ESP_LOGI(TAG, "%4d %s request ok", __LINE__, __func__);

EXIT:
  // clean
  esp_http_client_cleanup(client);

  if (post_data != NULL)
    free(post_data);

  /*
    if (module_http->response_content != NULL) {
      free(module_http->response_content);
      module_http->response_content = NULL;
    }
    */

  if (err != ESP_OK) {
    mt_module_http_utils_free_push_frame_res(res_out);
    return NULL;
  }

  return res_out;
}

static void mt_module_http_task_loop(mt_module_http_t *module_http) {
  // debug here
  int issue_module_token_interval = 30 * 1000; // 30s
  int show_module_retry_max = 10;
  int show_module_retry_count = 10;
  int show_module_interval = 30 * 1000; // 30s
  int heartbeat_max = 6;
  int heartbeat_count = 6;
  int heartbeat_interval = 30 * 1000; // 30s
  esp_err_t err = ESP_OK;
  module_t *module = NULL;

RESTART:
  // issue token loop
  while (true) {
    err = mt_module_http_actions_issue_module_token(module_http);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s mt_module_http_actions_issue_module_token failed",
               __LINE__, __func__);
    } else {
      ESP_LOGI(TAG, "%4d %s mt_module_http_actions_issue_module_token success",
               __LINE__, __func__);
      break;
    }

    vTaskDelay(issue_module_token_interval / portTICK_PERIOD_MS);
  }

  show_module_retry_count = show_module_retry_max;

  // show module loop
  while (true) {
    if (module != NULL) {
      mt_module_http_utils_free_module(module);
      module = NULL;
    }

    if (show_module_retry_count <= 0) {
      ESP_LOGE(TAG, "%4d %s show_module_retry_count get limit, restart loop",
               __LINE__, __func__);
      goto RESTART;
    }

    module = mt_module_http_actions_show_module(module_http);
    if (module == NULL) {
      ESP_LOGE(TAG, "%4d %s mt_module_http_actions_show_module failed",
               __LINE__, __func__);
      show_module_retry_count--;
    } else {
      ESP_LOGI(TAG, "%4d %s mt_module_http_actions_show_module success",
               __LINE__, __func__);
      module_http->module = module;

      break;
    }
    vTaskDelay(show_module_interval / portTICK_PERIOD_MS);
  }

  // heartbeat loop
  heartbeat_count = heartbeat_max;
  module_http->session_id =
      mt_utils_session_new_session(mt_utils_session_gen_startup_session(),
                                   mt_utils_session_gen_major_session());

  // debug here
  // module_http->session_id = 12345678;
  Session_id = module_http->session_id;
  mt_mqtt_update_session_id(Session_id);

  while (true) {
    if (heartbeat_count <= 0) {
      ESP_LOGE(TAG, "%4d %s heartbeat_count get limit, restart loop", __LINE__,
               __func__);
      goto RESTART;
    }

    err = mt_module_http_actions_heartbeat(module_http, module_http->module);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s mt_module_http_actions_heartbeat failed", __LINE__,
               __func__);
      heartbeat_count--;
    } else {
      ESP_LOGI(TAG, "%4d %s mt_module_http_actions_heartbeat success", __LINE__,
               __func__);
      heartbeat_count = heartbeat_max;
    }

    vTaskDelay(heartbeat_interval / portTICK_PERIOD_MS);
  }

  return;
}

void mt_module_http_task(mt_module_http_t *module_http) {
  // arg check
  if (module_http == NULL) {
    ESP_LOGE(TAG, "%4d %s module_http NULL", __LINE__, __func__);
    return;
  } else {
    if (module_http->host == NULL) {
      ESP_LOGE(TAG, "%4d %s module_http->host NULL", __LINE__, __func__);
      return;
    }

    if (module_http->port == 0) {
      ESP_LOGE(TAG, "%4d %s module_http->host NULL", __LINE__, __func__);
      return;
    }

    if (module_http->cred == NULL) {
      ESP_LOGE(TAG, "%4d %s module_http->cred NULL", __LINE__, __func__);
      return;
    } else {
      if (module_http->cred->id == NULL) {
        ESP_LOGE(TAG, "%4d %s module_http->cred->id NULL", __LINE__, __func__);
        return;
      }

      if (module_http->cred->secret == NULL) {
        ESP_LOGE(TAG, "%4d %s module_http->cred->secret NULL", __LINE__,
                 __func__);
        return;
      }
    }

    if (module_http->event_handler == NULL) {
      ESP_LOGE(TAG, "%4d %s module_http->event_handle  NULL", __LINE__,
               __func__);
      return;
    }
  }

  xTaskCreate((TaskFunction_t)mt_module_http_task_loop, "MT_MODULE_HTTP",
              8 * 1024, module_http, 10, NULL);
}

mt_module_http_t *mt_module_http_new(int mod_index_in) {
  mt_module_http_t *module_http = malloc(sizeof(mt_module_http_t));
  mt_nvs_host_t *host = NULL;
  mt_nvs_module_t *mod = malloc(sizeof(mt_nvs_module_t));

  host = mt_nvs_config_get_host_config();
  if (host == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_get_host_config failed", __LINE__,
             __func__);
    return NULL;
  }

  ESP_LOGI(TAG, "%4d %s host:%s http_port:%d mqtt_port:%s", __LINE__, __func__,
           host->host, host->http_port, host->mqtt_port);

  mod = mt_nvs_config_get_module(mod_index_in);
  if (mod == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_get_module index:%d failed", __LINE__,
             __func__, mod_index_in);
    return NULL;
  }

  module_http->host = host->host;
  module_http->port = host->http_port;
  if (host->use_ssl == true) {
    module_http->tran_type = HTTP_TRANSPORT_OVER_SSL;
  } else {
    module_http->tran_type = HTTP_TRANSPORT_OVER_TCP;
  }
  module_http->cred = malloc(sizeof(credential_t));
  module_http->cred->id = mod->id;
  module_http->cred->secret = mod->key;
  module_http->token = NULL;
  module_http->module = mt_module_http_utils_motule_t_init();
  module_http->module->name = mod->name;
  module_http->event_handler = module_http_event_handler;
  module_http->response_content = NULL;

  ESP_LOGI(TAG, "%4d %s module_name:%s cred_id:%s", __LINE__, __func__,
           mod->name, mod->id);

  MODULE_HTTP = module_http;
  mt_module_http_manage_add(module_http, mod_index_in);

  return module_http;
}
