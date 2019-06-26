#include "stdlib.h"

#include "cJSON.h"
#include "esp_log.h"

#include "esp_http_client.h"
#include "mt_http_client.h"
#include "mt_module_http.h"

// global define ==============================================================
static const char *TAG = "MT_MODULE_HTTP";

#define MAX_HTTP_RECV_BUFFER 512

char *MT_MODULE_HOST = "10.1.1.130";
int MT_MODULE_HOST_PORT = 8080;

// static func ================================================================
static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
  switch (evt->event_id)
  {
  case HTTP_EVENT_ERROR:
    ESP_LOGI(TAG, "%4d HTTP_EVENT_ERROR", __LINE__);
    break;
  case HTTP_EVENT_ON_CONNECTED:
    ESP_LOGI(TAG, "%4d HTTP_EVENT_ON_CONNECTED", __LINE__);
    break;
  case HTTP_EVENT_HEADER_SENT:
    ESP_LOGI(TAG, "%4d HTTP_EVENT_HEADER_SENT", __LINE__);
    break;
  case HTTP_EVENT_ON_HEADER:
    ESP_LOGI(TAG, "%4d HTTP_EVENT_ON_HEADER, key=%s, value=%s", __LINE__,
             evt->header_key, evt->header_value);
    break;
  case HTTP_EVENT_ON_DATA:
    ESP_LOGI(TAG, "%4d HTTP_EVENT_ON_DATA, len=%d, data=%s", __LINE__,
             evt->data_len, (char *)evt->data);
    if (!esp_http_client_is_chunked_response(evt->client))
    {
      // Write out data
      // printf("%.*s", evt->data_len, (char*)evt->data);
    }
    break;
  case HTTP_EVENT_ON_FINISH:
    ESP_LOGI(TAG, "%4d HTTP_EVENT_ON_FINISH", __LINE__);
    break;
  case HTTP_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "%4d HTTP_EVENT_DISCONNECTED", __LINE__);
    break;
  }
  return ESP_OK;
}

// global func ================================================================
esp_err_t mt_module_http_actions_issue_module_token(credential_t *cred_in,
                                                    int timestamp, int nonce,
                                                    char *hmac,
                                                    token_t *tkn_out)
{
  esp_err_t err;
  char *post_data = NULL;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *cred_in_json;

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
    return ESP_ERR_INVALID_ARG;
  }
  else
  {
    if (cred_in->id == NULL)
    {
      ESP_LOGE(TAG, "%4d %s cred_in->id is NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }
  }

  if (timestamp <= 0)
  {
    ESP_LOGE(TAG, "%4d %s timestamp error:%d", __LINE__, __func__, timestamp);
    return ESP_ERR_INVALID_ARG;
  }

  if (nonce <= 0)
  {
    ESP_LOGE(TAG, "%4d %s nonce error:%d", __LINE__, __func__, nonce);
    return ESP_ERR_INVALID_ARG;
  }

  if (hmac == NULL)
  {
    ESP_LOGE(TAG, "%4d %s hmac is NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
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
    return ESP_ERR_INVALID_RESPONSE;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 201)
  {
    ESP_LOGI(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    return ESP_ERR_HTTP_BASE;
  }

  ESP_LOGI(TAG, "%4d %s request ok", __LINE__, __func__);

  // clean
  esp_http_client_cleanup(client);

  return ESP_OK;
}

esp_err_t mt_module_http_actions_show_module(char *token_in,
                                             module_t *mdl_out)
{
  esp_err_t err;
  esp_http_client_handle_t client = NULL;

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
    return ESP_ERR_INVALID_ARG;
  }

  // request
  err = mt_http_client_post_request(client, token_in, NULL);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 200)
  {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    return ESP_ERR_HTTP_BASE;
  }

  ESP_LOGI(TAG, "%4d %s request ok", __LINE__, __func__);

  // clean
  esp_http_client_cleanup(client);

  return ESP_OK;
}

esp_err_t mt_module_http_actions_heartbeat(char *token_in, module_t *mod_in)
{
  esp_err_t err;
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
    return ESP_ERR_INVALID_ARG;
  }

  if (mod_in == NULL)
  {
    ESP_LOGE(TAG, "%4d %s mod_in is NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }
  else
  {
    if (mod_in->name == NULL)
    {
      ESP_LOGE(TAG, "%4d %s mod_in->name is NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }
  }

  // request post_data
  root = cJSON_CreateArray();
  cJSON_AddItemToObject(root, "module", mod_in_json = cJSON_CreateObject());
  cJSON_AddStringToObject(mod_in_json, "id", mod_in->id);
  post_data = cJSON_Print(root);
  cJSON_Delete(root);

  ESP_LOGI(TAG, "%4d %s post_data =%s", __LINE__, __func__, post_data);

  // request
  err = mt_http_client_post_request(client, token_in, post_data);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 204)
  {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    return ESP_ERR_HTTP_BASE;
  }

  ESP_LOGI(TAG, "%4d %s request ok", __LINE__, __func__);

  // clean
  esp_http_client_cleanup(client);

  return ESP_OK;
}

esp_err_t mt_module_http_actions_put_object(char *token_in, object_t *obj_in,
                                            char *content_in)
{
  esp_err_t err;
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
    return ESP_ERR_INVALID_ARG;
  }

  if (obj_in == NULL)
  {
    ESP_LOGE(TAG, "%4d %s obj_in is NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }
  else
  {
    if (obj_in->device == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->device is NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }
    else
    {
      if (obj_in->device->id == NULL)
      {
        ESP_LOGE(TAG, "%4d %s obj_in->device->id is NULL", __LINE__, __func__);
        return ESP_ERR_INVALID_ARG;
      }
    }

    if (obj_in->prefix == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->prefix is NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }

    if (obj_in->name == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->name is NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }
  }

  if (content_in == NULL)
  {
    ESP_LOGE(TAG, "%4d content_in is NULL", __LINE__);
    return ESP_ERR_INVALID_ARG;
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
  cJSON_Delete(NULL);

  ESP_LOGI(TAG, "%4d %s post_data =%s", __LINE__, __func__, post_data);

  // requset
  err = mt_http_client_post_request(client, token_in, post_data);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 204)
  {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    return ESP_ERR_HTTP_BASE;
  }

  ESP_LOGI(TAG, "%4d %s request ok", __LINE__, __func__);

  // clean
  esp_http_client_cleanup(client);

  return ESP_OK;
}

esp_err_t mt_module_http_actions_remove_object(char *token_in,
                                               object_t *obj_in)
{
  esp_err_t err;
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
    return ESP_ERR_INVALID_ARG;
  }

  if (obj_in == NULL)
  {
    ESP_LOGE(TAG, "%4d %s obj_in is NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }
  else
  {
    if (obj_in->device == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->device is NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }
    else
    {
      if (obj_in->device->id == NULL)
      {
        ESP_LOGE(TAG, "%4d %s obj_in->device->id is NULL", __LINE__, __func__);
        return ESP_ERR_INVALID_ARG;
      }
    }

    if (obj_in->prefix == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->prefix is NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }

    if (obj_in->name == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->name is NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
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
  cJSON_Delete(NULL);

  ESP_LOGI(TAG, "%4d %s post_data =%s", __LINE__, __func__, post_data);

  // requset
  err = mt_http_client_post_request(client, token_in, post_data);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 204)
  {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    return ESP_ERR_HTTP_BASE;
  }

  ESP_LOGI(TAG, "%4d %s request ok", __LINE__, __func__);

  // clean
  esp_http_client_cleanup(client);

  return ESP_OK;
}

esp_err_t mt_module_http_actions_rename_object(char *token_in, object_t *src_in,
                                               object_t *des_in)
{
  esp_err_t err;
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
    return ESP_ERR_INVALID_ARG;
  }

  if (src_in == NULL)
  {
    ESP_LOGE(TAG, "%4d %s src_in is NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }
  else
  {
    if (src_in->device == NULL)
    {
      ESP_LOGE(TAG, "%4d %s src_in->device is NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }
    else
    {
      if (src_in->device->id == NULL)
      {
        ESP_LOGE(TAG, "%4d %s src_in->device->id is NULL", __LINE__, __func__);
        return ESP_ERR_INVALID_ARG;
      }
    }

    if (src_in->prefix == NULL)
    {
      ESP_LOGE(TAG, "%4d %s src_in->prefix is NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }

    if (src_in->name == NULL)
    {
      ESP_LOGE(TAG, "%4d %s src_in->name is NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }
  }

  if (des_in == NULL)
  {
    ESP_LOGE(TAG, "%4d %s des_in is NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }
  else
  {
    if (des_in->device == NULL)
    {
      ESP_LOGE(TAG, "%4d %s des_in->device is NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }
    else
    {
      if (des_in->device->id == NULL)
      {
        ESP_LOGE(TAG, "%4d %s des_in->device->id is NULL", __LINE__, __func__);
        return ESP_ERR_INVALID_ARG;
      }
    }

    if (des_in->prefix == NULL)
    {
      ESP_LOGE(TAG, "%4d %s des_in->prefix is NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }

    if (des_in->name == NULL)
    {
      ESP_LOGE(TAG, "%4d %s des_in->name is NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
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
  cJSON_Delete(NULL);

  ESP_LOGI(TAG, "%4d %s post_data =%s", __LINE__, __func__, post_data);

  // requset
  err = mt_http_client_post_request(client, token_in, post_data);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 204)
  {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    return ESP_ERR_HTTP_BASE;
  }

  ESP_LOGI(TAG, "%4d %s request ok", __LINE__, __func__);

  // clean
  esp_http_client_cleanup(client);

  return ESP_OK;
}

esp_err_t mt_module_http_actions_get_object(char *token_in, object_t *obj_in,
                                            object_t *obj_out)
{
  esp_err_t err;
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
    return ESP_ERR_INVALID_ARG;
  }

  if (obj_in == NULL)
  {
    ESP_LOGE(TAG, "%4d %s obj_in is NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }
  else
  {
    if (obj_in->device == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->device is NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }
    else
    {
      if (obj_in->device->id == NULL)
      {
        ESP_LOGE(TAG, "%4d %s obj_in->device->id is NULL", __LINE__, __func__);
        return ESP_ERR_INVALID_ARG;
      }
    }

    if (obj_in->prefix == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->prefix is NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }

    if (obj_in->name == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->name is NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
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
  cJSON_Delete(NULL);

  ESP_LOGI(TAG, "%4d %s post_data =%s", __LINE__, __func__, post_data);

  // requset
  err = mt_http_client_post_request(client, token_in, post_data);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 200)
  {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    return ESP_ERR_HTTP_BASE;
  }

  ESP_LOGI(TAG, "%4d %s request ok", __LINE__, __func__);

  // clean
  esp_http_client_cleanup(client);

  return ESP_OK;
}

esp_err_t mt_module_http_actions_get_object_content(char *token_in,
                                                    object_t *obj_in,
                                                    char *content_out)
{
  esp_err_t err;
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
    return ESP_ERR_INVALID_ARG;
  }

  if (obj_in == NULL)
  {
    ESP_LOGE(TAG, "%4d %s obj_in is NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }
  else
  {
    if (obj_in->device == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->device is NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }
    else
    {
      if (obj_in->device->id == NULL)
      {
        ESP_LOGE(TAG, "%4d %s obj_in->device->id is NULL", __LINE__, __func__);
        return ESP_ERR_INVALID_ARG;
      }
    }

    if (obj_in->prefix == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->prefix is NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }

    if (obj_in->name == NULL)
    {
      ESP_LOGE(TAG, "%4d %s obj_in->name is NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
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
  cJSON_Delete(NULL);

  ESP_LOGI(TAG, "%4d %s post_data =%s", __LINE__, __func__, post_data);

  // requset
  err = mt_http_client_post_request(client, token_in, post_data);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 200)
  {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    return ESP_ERR_HTTP_BASE;
  }

  ESP_LOGI(TAG, "%4d %s request ok", __LINE__, __func__);

  // clean
  esp_http_client_cleanup(client);

  return ESP_OK;
}

esp_err_t mt_module_http_actions_list_objects(char *token_in, object_t *obj_in,
                                              object_t **objs_out)
{
  esp_err_t err;
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
    return ESP_ERR_INVALID_ARG;
  }

  if (obj_in == NULL)
  {
    ESP_LOGE(TAG, "%4d %s obj_in is NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
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
  cJSON_Delete(NULL);

  ESP_LOGI(TAG, "%4d %s post_data =%s", __LINE__, __func__, post_data);

  // requset
  err = mt_http_client_post_request(client, token_in, post_data);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s mt_http_client_post_request failed", __LINE__,
             __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // check res code
  int res_code = esp_http_client_get_status_code(client);
  if (res_code != 204)
  {
    ESP_LOGE(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    return ESP_ERR_HTTP_BASE;
  }

  ESP_LOGI(TAG, "%4d %s request ok", __LINE__, __func__);

  // clean
  esp_http_client_cleanup(client);

  return ESP_OK;
}
