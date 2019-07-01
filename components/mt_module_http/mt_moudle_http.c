#include "stdlib.h"
#include "string.h"

#include "cJSON.h"
#include "esp_http_client.h"
#include "esp_log.h"

#include "mt_http_client.h"
#include "mt_module_http.h"
#include "mt_module_http_utils.h"
#include "mt_utils.h"

// global define ==============================================================
static const char *TAG = "MT_MODULE_HTTP";

#define MAX_HTTP_RECV_BUFFER 512

char *MT_MODULE_HOST = "10.1.1.130";
int MT_MODULE_HOST_PORT = 8080;

char *MT_MODULE_RESPONSE_CONTENT = NULL;
int MT_MODULE_RESPONSE_CONTENT_SIZE = 0;

// static func ================================================================
static esp_err_t mt_module_save_content(char *content, int size)
{
  if (MT_MODULE_RESPONSE_CONTENT != NULL)
  {
    free(MT_MODULE_RESPONSE_CONTENT);
  }

  if (size <= 0)
  {
    ESP_LOGE(TAG, "%4d %s content size =%d", __LINE__, __func__, size);
    return ESP_ERR_INVALID_ARG;
  }

  MT_MODULE_RESPONSE_CONTENT = malloc(size + 1);

  memcpy(MT_MODULE_RESPONSE_CONTENT, content, size);
  MT_MODULE_RESPONSE_CONTENT[size] = '\0';
  MT_MODULE_RESPONSE_CONTENT_SIZE = size;

  ESP_LOGI(TAG, "%4d save content, size=%d, string=%s", __LINE__, size,
           MT_MODULE_RESPONSE_CONTENT);

  return ESP_OK;
}

static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
  switch (evt->event_id)
  {
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
    if (!esp_http_client_is_chunked_response(evt->client))
    {
      esp_err_t err;

      ESP_LOGD(TAG, "%4d HTTP_EVENT_ON_DATA, len=%d, data=%s", __LINE__,
               evt->data_len, (char *)evt->data);
      err = mt_module_save_content((char *)evt->data, evt->data_len);
      if (err != ESP_OK)
      {
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
token_t *mt_module_http_actions_issue_module_token(credential_t *cred_in,
                                                   int timestamp, int nonce,
                                                   char *hmac)
{
  esp_err_t err = ESP_OK;
  char *post_data = NULL;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *cred_in_json;
  token_t *tkn_out = NULL;

  esp_http_client_config_t config = {
      .host = MT_MODULE_HOST,
      .port = MT_MODULE_HOST_PORT,
      .path = "/actions/issue_module_token",
      .event_handler = _http_event_handler,
  };

  client = esp_http_client_init(&config);

  // check argument
  if (cred_in == NULL)
  {
    ESP_LOGE(TAG, "%4d %s cred_in is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  else
  {
    if (cred_in->id == NULL)
    {
      ESP_LOGE(TAG, "%4d %s cred_in->id is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  if (timestamp <= 0)
  {
    ESP_LOGE(TAG, "%4d %s timestamp EXIT:%d", __LINE__, __func__, timestamp);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (nonce <= 0)
  {
    ESP_LOGE(TAG, "%4d %s nonce EXIT:%d", __LINE__, __func__, nonce);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (hmac == NULL)
  {
    ESP_LOGE(TAG, "%4d %s hmac is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // request post_data
  root = cJSON_CreateArray();
  cJSON_AddItemToObject(root, "credential",
                        cred_in_json = cJSON_CreateObject());
  cJSON_AddStringToObject(cred_in_json, "id", cred_in->id);
  cJSON_AddNumberToObject(root, "timestamp", timestamp);
  cJSON_AddNumberToObject(root, "nonce", nonce);
  cJSON_AddStringToObject(root, "hmac", hmac);
  post_data = cJSON_Print(root);
  cJSON_Delete(root);

  ESP_LOGI(TAG, "%4d %s post_data =%s", __LINE__, __func__, post_data);

  // request
  err = mt_http_client_post_request(client, NULL, post_data);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 201)
  {
    ESP_LOGI(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  goto EXIT;
  // parse res content
  int content_size = esp_http_client_get_content_length(client);
  if (content_size != 0)
  {
    if (content_size != MT_MODULE_RESPONSE_CONTENT_SIZE)
    {
      ESP_LOGE(TAG, "%4d content_size %d != MT_MODULE_RESPONSE_CONTENT_SIZE %d",
               __LINE__, content_size, MT_MODULE_RESPONSE_CONTENT_SIZE);
      err = ESP_ERR_HTTP_BASE;
      goto EXIT;
    }
    else
    {
      tkn_out =
          mt_module_http_utils_parse_token_res(MT_MODULE_RESPONSE_CONTENT);
      if (tkn_out == NULL)
      {
        ESP_LOGE(TAG, "%4d mt_module_http_utils_parse_token_res failed code=%d",
                 __LINE__, err);
        err = ESP_ERR_HTTP_BASE;
        goto EXIT;
      }

      if (tkn_out == NULL)
      {
        ESP_LOGE(TAG, "%4d %s token NULL", __LINE__, __func__);
        err = ESP_ERR_HTTP_BASE;
        goto EXIT;
      }
      else
      {
        if (tkn_out->text == NULL)
        {
          ESP_LOGE(TAG, "%4d %s token->text NULL", __LINE__, __func__);
          err = ESP_ERR_HTTP_BASE;
          goto EXIT;
        }
      }
    }
  }

  ESP_LOGI(TAG, "%4d %s request ok", __LINE__, __func__);
  if (tkn_out != NULL)
  {
    if (tkn_out->text != NULL)
    {
      ESP_LOGW(TAG, "%4d %s token=%s", __LINE__, __func__, tkn_out->text);
    }
  }

EXIT:
  // clean
  esp_http_client_cleanup(client);
  if (err != ESP_OK)
  {
    if (tkn_out != NULL)
    {
      mt_module_http_utils_free_token(tkn_out);
    }

    return NULL;
  }

  return tkn_out;
}

module_t *mt_module_http_actions_show_module(char *token_in)
{
  esp_err_t err;
  esp_http_client_handle_t client = NULL;
  module_t *mdl_out = NULL;
  esp_http_client_config_t config = {
      .host = MT_MODULE_HOST,
      .port = MT_MODULE_HOST_PORT,
      .path = "/actions/show_module",
      .event_handler = _http_event_handler,
  };

  client = esp_http_client_init(&config);

  // check argument
  if (token_in == NULL)
  {
    ESP_LOGE(TAG, "%4d token_in is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // request
  err = mt_http_client_post_request(client, token_in, NULL);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 200)
  {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    err = ESP_ERR_HTTP_BASE;
    goto EXIT;
  }

  // parse res content
  int content_size = esp_http_client_get_content_length(client);
  if (content_size != 0)
  {
    if (content_size != MT_MODULE_RESPONSE_CONTENT_SIZE)
    {
      ESP_LOGE(TAG, "%4d content_size %d != MT_MODULE_RESPONSE_CONTENT_SIZE %d",
               __LINE__, content_size, MT_MODULE_RESPONSE_CONTENT_SIZE);
      err = ESP_ERR_HTTP_BASE;
      goto EXIT;
    }
    else
    {
      mdl_out =
          mt_module_http_uitls_parse_module_res(MT_MODULE_RESPONSE_CONTENT);
      if (mdl_out == NULL)
      {
        ESP_LOGE(TAG, "%4d mt_module_http_utils_parse_token_res failed code=%d",
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

  if (err != ESP_OK)
  {
    if (mdl_out != NULL)
    {
      mt_module_http_utils_free_module(mdl_out);
    }

    return NULL;
  }

  return mdl_out;
}

esp_err_t mt_module_http_actions_heartbeat(char *token_in, uint64_t session_id,
                                           module_t *mod_in)
{
  esp_err_t err = ESP_OK;
  char *post_data = NULL;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *mod_in_json;

  esp_http_client_config_t config = {
      .host = MT_MODULE_HOST,
      .port = MT_MODULE_HOST_PORT,
      .path = "/actions/heartbeat",
      .event_handler = _http_event_handler,
  };

  client = esp_http_client_init(&config);

  // check argument
  if (token_in == NULL)
  {
    ESP_LOGE(TAG, "%4d token_in is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (session_id == 0)
  {
    ESP_LOGE(TAG, "%4d session_id is 0", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (mod_in == NULL)
  {
    ESP_LOGE(TAG, "%4d %s mod_in is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  else
  {
    if (mod_in->name == NULL)
    {
      ESP_LOGE(TAG, "%4d %s mod_in->name is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  // request post_data
  root = cJSON_CreateArray();
  cJSON_AddItemToObject(root, "module", mod_in_json = cJSON_CreateObject());
  cJSON_AddStringToObject(mod_in_json, "id", mod_in->id);
  post_data = cJSON_Print(root);
  cJSON_Delete(root);

  ESP_LOGI(TAG, "%4d %s post_data =%s", __LINE__, __func__, post_data);

  // request extra header
  uint8_t str_size = 0;
  char *session_id_str = NULL;

  session_id_str = mt_utils_int64_to_string(session_id, &str_size);
  err = esp_http_client_set_header(
      client, "MT-Module-Session",
      session_id_str);

  if (err != ESP_OK)
  {
    ESP_LOGE(TAG,
             "%4d esp_http_client_set_header MT-Module-Session failed code=%d",
             __LINE__, err);
    goto EXIT;
  }

  // request
  err = mt_http_client_post_request(client, token_in, post_data);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 204)
  {
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

esp_err_t mt_module_http_actions_put_object(char *token_in, object_t *obj_in,
                                            char *content_in)
{
  esp_err_t err = ESP_OK;
  char *post_data;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *obj_in_json, *obj_in_device_json;

  esp_http_client_config_t config = {
      .host = MT_MODULE_HOST,
      .port = MT_MODULE_HOST_PORT,
      .path = "/actions/put_object",
      .event_handler = _http_event_handler,
  };

  client = esp_http_client_init(&config);

  // check argument
  if (token_in == NULL)
  {
    ESP_LOGE(TAG, "%4d token_in is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (obj_in == NULL)
  {
    ESP_LOGE(TAG, "%4d %s obj_in is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  else
  {
    if (obj_in->device == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->device is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
    else
    {
      if (obj_in->device->id == NULL)
      {
        ESP_LOGE(TAG, "%4d %s obj_in->device->id is NULL", __LINE__, __func__);
        err = ESP_ERR_INVALID_ARG;
        goto EXIT;
      }
    }

    if (obj_in->prefix == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->prefix is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }

    if (obj_in->name == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->name is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  if (content_in == NULL)
  {
    ESP_LOGE(TAG, "%4d content_in is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // request post_data
  root = cJSON_CreateArray();
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
  err = mt_http_client_post_request(client, token_in, post_data);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 204)
  {
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

esp_err_t mt_module_http_actions_remove_object(char *token_in,
                                               object_t *obj_in)
{
  esp_err_t err = ESP_OK;
  char *post_data;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *obj_in_json, *obj_in_device_json;

  esp_http_client_config_t config = {
      .host = MT_MODULE_HOST,
      .port = MT_MODULE_HOST_PORT,
      .path = "/actions/remove_object",
      .event_handler = _http_event_handler,
  };

  client = esp_http_client_init(&config);

  // check argument
  if (token_in == NULL)
  {
    ESP_LOGE(TAG, "%4d token_in is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (obj_in == NULL)
  {
    ESP_LOGE(TAG, "%4d %s obj_in is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  else
  {
    if (obj_in->device == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->device is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
    else
    {
      if (obj_in->device->id == NULL)
      {
        ESP_LOGE(TAG, "%4d %s obj_in->device->id is NULL", __LINE__, __func__);
        err = ESP_ERR_INVALID_ARG;
        goto EXIT;
      }
    }

    if (obj_in->prefix == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->prefix is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }

    if (obj_in->name == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->name is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  // request post_data
  root = cJSON_CreateArray();
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
  err = mt_http_client_post_request(client, token_in, post_data);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 204)
  {
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

esp_err_t mt_module_http_actions_rename_object(char *token_in, object_t *src_in,
                                               object_t *des_in)
{
  esp_err_t err = ESP_OK;
  char *post_data;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *src_in_json, *src_in_device_json, *des_in_json,
      *des_in_device_json;

  esp_http_client_config_t config = {
      .host = MT_MODULE_HOST,
      .port = MT_MODULE_HOST_PORT,
      .path = "/actions/rename_object",
      .event_handler = _http_event_handler,
  };

  client = esp_http_client_init(&config);

  // check argument
  if (token_in == NULL)
  {
    ESP_LOGE(TAG, "%4d token_in is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (src_in == NULL)
  {
    ESP_LOGE(TAG, "%4d %s src_in is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  else
  {
    if (src_in->device == NULL)
    {
      ESP_LOGE(TAG, "%4d %s src_in->device is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
    else
    {
      if (src_in->device->id == NULL)
      {
        ESP_LOGE(TAG, "%4d %s src_in->device->id is NULL", __LINE__, __func__);
        err = ESP_ERR_INVALID_ARG;
        goto EXIT;
      }
    }

    if (src_in->prefix == NULL)
    {
      ESP_LOGE(TAG, "%4d %s src_in->prefix is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }

    if (src_in->name == NULL)
    {
      ESP_LOGE(TAG, "%4d %s src_in->name is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  if (des_in == NULL)
  {
    ESP_LOGE(TAG, "%4d %s des_in is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  else
  {
    if (des_in->device == NULL)
    {
      ESP_LOGE(TAG, "%4d %s des_in->device is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
    else
    {
      if (des_in->device->id == NULL)
      {
        ESP_LOGE(TAG, "%4d %s des_in->device->id is NULL", __LINE__, __func__);
        err = ESP_ERR_INVALID_ARG;
        goto EXIT;
      }
    }

    if (des_in->prefix == NULL)
    {
      ESP_LOGE(TAG, "%4d %s des_in->prefix is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }

    if (des_in->name == NULL)
    {
      ESP_LOGE(TAG, "%4d %s des_in->name is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  // request post_data
  root = cJSON_CreateArray();
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
  err = mt_http_client_post_request(client, token_in, post_data);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 204)
  {
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

object_t *mt_module_http_actions_get_object(char *token_in, object_t *obj_in)
{
  esp_err_t err = ESP_OK;
  object_t *obj_out = NULL;
  char *post_data;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *obj_in_json, *obj_in_device_json;

  esp_http_client_config_t config = {
      .host = MT_MODULE_HOST,
      .port = MT_MODULE_HOST_PORT,
      .path = "/actions/get_object",
      .event_handler = _http_event_handler,
  };

  client = esp_http_client_init(&config);

  // check argument
  if (token_in == NULL)
  {
    ESP_LOGE(TAG, "%4d token_in is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (obj_in == NULL)
  {
    ESP_LOGE(TAG, "%4d %s obj_in is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  else
  {
    if (obj_in->device == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->device is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
    else
    {
      if (obj_in->device->id == NULL)
      {
        ESP_LOGE(TAG, "%4d %s obj_in->device->id is NULL", __LINE__, __func__);
        err = ESP_ERR_INVALID_ARG;
        goto EXIT;
      }
    }

    if (obj_in->prefix == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->prefix is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }

    if (obj_in->name == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->name is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  // request post_data
  root = cJSON_CreateArray();
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
  err = mt_http_client_post_request(client, token_in, post_data);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 200)
  {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    err = ESP_ERR_HTTP_BASE;
    goto EXIT;
  }

  // parse res content
  int content_size = esp_http_client_get_content_length(client);
  if (content_size != 0)
  {
    if (content_size != MT_MODULE_RESPONSE_CONTENT_SIZE)
    {
      ESP_LOGE(TAG, "%4d content_size %d != MT_MODULE_RESPONSE_CONTENT_SIZE %d",
               __LINE__, content_size, MT_MODULE_RESPONSE_CONTENT_SIZE);
      err = ESP_ERR_HTTP_BASE;
      goto EXIT;
    }
    else
    {
      obj_out =
          mt_module_http_utils_parse_object_res(MT_MODULE_RESPONSE_CONTENT);
      if (obj_out == NULL)
      {
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

  if (err != ESP_OK)
  {
    if (obj_out != NULL)
    {
      mt_module_http_utils_free_object(obj_out);
    }

    return NULL;
  }

  return obj_out;
}

char *mt_module_http_actions_get_object_content(char *token_in,
                                                object_t *obj_in)
{
  esp_err_t err = ESP_OK;
  char *content_out = NULL;
  char *post_data;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *obj_in_json, *obj_in_device_json;

  esp_http_client_config_t config = {
      .host = MT_MODULE_HOST,
      .port = MT_MODULE_HOST_PORT,
      .path = "/actions/get_object_content",
      .event_handler = _http_event_handler,
  };

  client = esp_http_client_init(&config);

  // check argument
  if (token_in == NULL)
  {
    ESP_LOGE(TAG, "%4d token_in is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (obj_in == NULL)
  {
    ESP_LOGE(TAG, "%4d %s obj_in is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  else
  {
    if (obj_in->device == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->device is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
    else
    {
      if (obj_in->device->id == NULL)
      {
        ESP_LOGE(TAG, "%4d %s obj_in->device->id is NULL", __LINE__, __func__);
        err = ESP_ERR_INVALID_ARG;
        goto EXIT;
      }
    }

    if (obj_in->prefix == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->prefix is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }

    if (obj_in->name == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->name is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  // request post_data
  root = cJSON_CreateArray();
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
  err = mt_http_client_post_request(client, token_in, post_data);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 200)
  {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    err = ESP_ERR_HTTP_BASE;
    goto EXIT;
  }

  // parse res content
  int content_size = esp_http_client_get_content_length(client);
  if (content_size != 0)
  {
    if (content_size != MT_MODULE_RESPONSE_CONTENT_SIZE)
    {
      ESP_LOGE(TAG, "%4d content_size %d != MT_MODULE_RESPONSE_CONTENT_SIZE %d",
               __LINE__, content_size, MT_MODULE_RESPONSE_CONTENT_SIZE);
      err = ESP_ERR_HTTP_BASE;
      goto EXIT;
    }
    else
    {
      content_out =
          mt_module_http_utils_parse_content_res(MT_MODULE_RESPONSE_CONTENT);
      if (content_out == NULL)
      {
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

  if (err != ESP_OK)
  {
    if (content_out != NULL)
      free(content_out);

    return NULL;
  }

  return content_out;
}

uint8_t *mt_module_http_actions_list_objects(char *token_in, object_t *obj_in,
                                             int *objs_num)
{
  esp_err_t err = ESP_OK;
  uint8_t *objs_out = NULL;
  char *post_data;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *obj_in_json, *obj_in_device_json;

  esp_http_client_config_t config = {
      .host = MT_MODULE_HOST,
      .port = MT_MODULE_HOST_PORT,
      .path = "/actions/list_objects",
      .event_handler = _http_event_handler,
  };

  client = esp_http_client_init(&config);

  // check argument
  if (token_in == NULL)
  {
    ESP_LOGE(TAG, "%4d token_in is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (obj_in == NULL)
  {
    ESP_LOGE(TAG, "%4d %s obj_in is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // request post_data
  root = cJSON_CreateArray();
  cJSON_AddItemToObject(root, "object", obj_in_json = cJSON_CreateObject());
  if (obj_in->device != NULL)
  {
    cJSON_AddItemToObject(obj_in_json, "device",
                          obj_in_device_json = cJSON_CreateObject());
    if (obj_in->device->id != NULL)
    {
      cJSON_AddStringToObject(obj_in_device_json, "id", obj_in->device->id);
    }
  }
  if (obj_in->prefix != NULL)
  {
    cJSON_AddStringToObject(obj_in_json, "prefix", obj_in->prefix);
  }
  if (obj_in->name != NULL)
  {
    cJSON_AddStringToObject(obj_in_json, "name", obj_in->name);
  }
  post_data = cJSON_Print(root);
  cJSON_Delete(root);

  ESP_LOGI(TAG, "%4d %s post_data =%s", __LINE__, __func__, post_data);

  // requset
  err = mt_http_client_post_request(client, token_in, post_data);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 204)
  {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    err = ESP_ERR_HTTP_BASE;
    goto EXIT;
  }

  // parse res content
  int content_size = esp_http_client_get_content_length(client);
  if (content_size != 0)
  {
    if (content_size != MT_MODULE_RESPONSE_CONTENT_SIZE)
    {
      ESP_LOGE(TAG, "%4d content_size %d != MT_MODULE_RESPONSE_CONTENT_SIZE %d",
               __LINE__, content_size, MT_MODULE_RESPONSE_CONTENT_SIZE);
      err = ESP_ERR_HTTP_BASE;
      goto EXIT;
    }
    else
    {
      objs_out = mt_module_http_utils_parse_objects_res(
          MT_MODULE_RESPONSE_CONTENT, objs_num);
      if (objs_out == NULL)
      {
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

  if (err != ESP_OK)
  {
    if (objs_out != NULL)
    {
      free(objs_out);
    }

    return NULL;
  }

  return objs_out;
}
