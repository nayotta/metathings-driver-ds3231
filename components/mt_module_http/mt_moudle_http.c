#include "stdlib.h"
#include "string.h"

#include "cJSON.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "mt_http_client.h"
#include "mt_module_http.h"
#include "mt_module_http_utils.h"
#include "mt_utils.h"
#include "mt_utils_login.h"
#include "mt_utils_session.h"

// global define ==============================================================
static const char *TAG = "MT_MODULE_HTTP";

#define MAX_HTTP_RECV_BUFFER 512

// global func ================================================================
esp_err_t mt_module_http_actions_issue_module_token(
    mt_module_http_t *module_http)
{
  esp_err_t err = ESP_OK;
  char *post_data = NULL;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *cred_in_json;
  uint8_t *time_stamp = NULL;
  uint8_t time_stamp_size = 0;
  char *time_stamp_str = NULL;
  uint32_t nonce = 0;
  unsigned char *hmac = NULL;
  token_t *tkn_out = NULL;
  time_t now = 0;

  esp_http_client_config_t config = {
      .host = module_http->host,
      .port = module_http->port,
      .path = "/v1/device_cloud/actions/issue_module_token",
      .event_handler = module_http->event_handler,
  };

  client = esp_http_client_init(&config);

  // check argument
  if (module_http == NULL)
  {
    ESP_LOGE(TAG, "%4d %s module_http is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  else
  {
    if (module_http->cred == NULL)
    {
      ESP_LOGE(TAG, "%4d %s module_http->cred_id is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
    else
    {
      if (module_http->cred->id == NULL)
      {
        ESP_LOGE(TAG, "%4d %s module_http->cred->id is NULL", __LINE__,
                 __func__);
        err = ESP_ERR_INVALID_ARG;
        goto EXIT;
      }

      if (module_http->cred->secret == NULL)
      {
        ESP_LOGE(TAG, "%4d %s module_http->cred->secret is NULL", __LINE__,
                 __func__);
        err = ESP_ERR_INVALID_ARG;
        goto EXIT;
      }
    }
  }

  // count hmac
  now = mt_utils_login_get_time_now();
  time_stamp = mt_utils_login_get_time_rfc3339nano_string(now, &time_stamp_size);
  if (time_stamp == NULL)
  {
    ESP_LOGE(TAG, "%4d %s mt_utils_login_get_time_rfc3339nano_string failed",
             __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }
  time_stamp_str = mt_utils_login_time_to_ms_string(now);

  nonce = mt_utils_login_get_nonce();

  hmac = mt_hmac_sha256_base64(
      (uint8_t *)module_http->cred->secret, strlen(module_http->cred->secret),
      (uint8_t *)module_http->cred->id, strlen(module_http->cred->id),
      (uint8_t *)time_stamp_str, strlen(time_stamp_str), nonce);
  if (hmac == NULL)
  {
    ESP_LOGE(TAG, "%4d %s mt_hmac_sha256 error", __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // request post_data
  root = cJSON_CreateObject();
  cJSON_AddItemToObject(root, "credential",
                        cred_in_json = cJSON_CreateObject());
  cJSON_AddStringToObject(cred_in_json, "id", module_http->cred->id);
  cJSON_AddStringToObject(root, "timestamp", (char *)time_stamp);
  cJSON_AddNumberToObject(root, "nonce", nonce);
  cJSON_AddStringToObject(root, "hmac", (char *)hmac);
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
  if (res_code != 200)
  {
    ESP_LOGI(TAG, "%4d %s requst failed code:%d", __LINE__, __func__, res_code);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // parse res content
  int content_size = esp_http_client_get_content_length(client);
  if (content_size != 0)
  {
    if (content_size != module_http->response_content_size)
    {
      ESP_LOGE(TAG,
               "%4d content_size %d != module_http->response_content_size %d",
               __LINE__, content_size, module_http->response_content_size);
      err = ESP_ERR_HTTP_BASE;
      goto EXIT;
    }
    else
    {
      tkn_out =
          mt_module_http_utils_parse_token_res(module_http->response_content);
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

  if (module_http->token != NULL)
  {
    ESP_LOGE(TAG, "module_http->token free");
    free(module_http->token);
  }

  module_http->token = malloc(strlen(tkn_out->text) + 1);
  module_http->token[strlen(tkn_out->text)] = '\0';
  memcpy(module_http->token, tkn_out->text, strlen(tkn_out->text));

EXIT:
  if (time_stamp != NULL)
    free(time_stamp);

  if (hmac != NULL)
    free(hmac);

  // clean
  esp_http_client_cleanup(client);
  if (err != ESP_OK)
  {
    if (tkn_out != NULL)
    {
      mt_module_http_utils_free_token(tkn_out);
    }
  }

  return err;
}

module_t *mt_module_http_actions_show_module(mt_module_http_t *module_http)
{
  esp_err_t err;
  esp_http_client_handle_t client = NULL;
  module_t *mdl_out = NULL;
  esp_http_client_config_t config = {
      .host = module_http->host,
      .port = module_http->port,
      .path = "/v1/device_cloud/actions/show_module",
      .event_handler = module_http->event_handler,
  };

  client = esp_http_client_init(&config);

  // check argument
  if (module_http->token == NULL)
  {
    ESP_LOGE(TAG, "%4d module_http->token is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // request
  err = mt_http_client_post_request(client, module_http->token, NULL);
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
    if (content_size != module_http->response_content_size)
    {
      ESP_LOGE(TAG, "%4d content_size %d != response_content_size %d", __LINE__,
               content_size, module_http->response_content_size);
      err = ESP_ERR_HTTP_BASE;
      goto EXIT;
    }
    else
    {
      mdl_out =
          mt_module_http_uitls_parse_module_res(module_http->response_content);
      if (mdl_out == NULL)
      {
        ESP_LOGE(TAG, "%4d mt_module_http_utils_parse_token_res failed code=%d",
                 __LINE__, err);
        err = ESP_ERR_HTTP_BASE;
        goto EXIT;
      }
      else
      {
        if (mdl_out->id == NULL)
        {
          ESP_LOGE(TAG, "%4d mt_module_http_utils_parse_token_res module_id NULL",
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

esp_err_t mt_module_http_actions_heartbeat(mt_module_http_t *module_http,
                                           module_t *mod_in)
{
  esp_err_t err = ESP_OK;
  char *post_data = NULL;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *mod_in_json;

  esp_http_client_config_t config = {
      .host = module_http->host,
      .port = module_http->port,
      .path = "/v1/device_cloud/actions/heartbeat",
      .event_handler = module_http->event_handler,
  };

  client = esp_http_client_init(&config);

  // check argument
  if (module_http->token == NULL)
  {
    ESP_LOGE(TAG, "%4d token is NULL", __LINE__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (module_http->session_id == 0)
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
  root = cJSON_CreateObject();
  //cJSON_AddItemToObject(root, "module", mod_in_json = cJSON_CreateObject());
  //cJSON_AddStringToObject(mod_in_json, "name", mod_in->name);
  post_data = cJSON_Print(root);
  cJSON_Delete(root);

  ESP_LOGI(TAG, "%4d %s post_data =%s", __LINE__, __func__, post_data);

  // request extra header
  uint8_t str_size = 0;
  char *session_id_str = NULL;

  session_id_str = mt_utils_int64_to_string(module_http->session_id, &str_size);
  err = esp_http_client_set_header(client, "MT-Module-Session", session_id_str);

  if (err != ESP_OK)
  {
    ESP_LOGE(TAG,
             "%4d esp_http_client_set_header MT-Module-Session failed code=%d",
             __LINE__, err);
    goto EXIT;
  }

  // request
  err = mt_http_client_post_request(client, module_http->token, post_data);
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

esp_err_t mt_module_http_actions_put_object(mt_module_http_t *module_http,
                                            object_t *obj_in,
                                            char *content_in)
{
  esp_err_t err = ESP_OK;
  char *post_data;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *obj_in_json, *obj_in_device_json;

  esp_http_client_config_t config = {
      .host = module_http->host,
      .port = module_http->port,
      .path = "/v1/device_cloud/actions/put_object",
      .event_handler = module_http->event_handler,
  };

  client = esp_http_client_init(&config);

  // check argument
  if (module_http->token == NULL)
  {
    ESP_LOGE(TAG, "%4d token is NULL", __LINE__);
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

esp_err_t mt_module_http_actions_remove_object(mt_module_http_t *module_http,
                                               object_t *obj_in)
{
  esp_err_t err = ESP_OK;
  char *post_data;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *obj_in_json, *obj_in_device_json;

  esp_http_client_config_t config = {
      .host = module_http->host,
      .port = module_http->port,
      .path = "/v1/device_cloud/actions/remove_object",
      .event_handler = module_http->event_handler,
  };

  client = esp_http_client_init(&config);

  // check argument
  if (module_http->token == NULL)
  {
    ESP_LOGE(TAG, "%4d token is NULL", __LINE__);
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

esp_err_t mt_module_http_actions_rename_object(mt_module_http_t *module_http,
                                               object_t *src_in,
                                               object_t *des_in)
{
  esp_err_t err = ESP_OK;
  char *post_data;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *src_in_json, *src_in_device_json, *des_in_json,
      *des_in_device_json;

  esp_http_client_config_t config = {
      .host = module_http->host,
      .port = module_http->port,
      .path = "/v1/device_cloud/actions/rename_object",
      .event_handler = module_http->event_handler,
  };

  client = esp_http_client_init(&config);

  // check argument
  if (module_http->token == NULL)
  {
    ESP_LOGE(TAG, "%4d token is NULL", __LINE__);
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

object_t *mt_module_http_actions_get_object(mt_module_http_t *module_http,
                                            object_t *obj_in)
{
  esp_err_t err = ESP_OK;
  object_t *obj_out = NULL;
  char *post_data;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *obj_in_json, *obj_in_device_json;

  esp_http_client_config_t config = {
      .host = module_http->host,
      .port = module_http->port,
      .path = "/v1/device_cloud/actions/get_object",
      .event_handler = module_http->event_handler,
  };

  client = esp_http_client_init(&config);

  // check argument
  if (module_http->token == NULL)
  {
    ESP_LOGE(TAG, "%4d token is NULL", __LINE__);
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
    if (content_size != module_http->response_content_size)
    {
      ESP_LOGE(TAG, "%4d content_size %d != response_content_size %d", __LINE__,
               content_size, module_http->response_content_size);
      err = ESP_ERR_HTTP_BASE;
      goto EXIT;
    }
    else
    {
      obj_out =
          mt_module_http_utils_parse_object_res(module_http->response_content);
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

char *mt_module_http_actions_get_object_content(mt_module_http_t *module_http,
                                                object_t *obj_in)
{
  esp_err_t err = ESP_OK;
  char *content_out = NULL;
  char *post_data;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *obj_in_json, *obj_in_device_json;

  esp_http_client_config_t config = {
      .host = module_http->host,
      .port = module_http->port,
      .path = "/v1/device_cloud/actions/get_object_content",
      .event_handler = module_http->event_handler,
  };

  client = esp_http_client_init(&config);

  // check argument
  if (module_http->token == NULL)
  {
    ESP_LOGE(TAG, "%4d token is NULL", __LINE__);
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
    if (content_size != module_http->response_content_size)
    {
      ESP_LOGE(TAG, "%4d content_size %d != response_content_size %d", __LINE__,
               content_size, module_http->response_content_size);
      err = ESP_ERR_HTTP_BASE;
      goto EXIT;
    }
    else
    {
      content_out =
          mt_module_http_utils_parse_content_res(module_http->response_content);
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

uint8_t *mt_module_http_actions_list_objects(mt_module_http_t *module_http,
                                             object_t *obj_in, int *objs_num)
{
  esp_err_t err = ESP_OK;
  uint8_t *objs_out = NULL;
  char *post_data;
  esp_http_client_handle_t client = NULL;
  cJSON *root, *obj_in_json, *obj_in_device_json;

  esp_http_client_config_t config = {
      .host = module_http->host,
      .port = module_http->port,
      .path = "/v1/device_cloud/actions/list_objects",
      .event_handler = module_http->event_handler,
  };

  client = esp_http_client_init(&config);

  // check argument
  if (module_http->token == NULL)
  {
    ESP_LOGE(TAG, "%4d token is NULL", __LINE__);
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
  root = cJSON_CreateObject();
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
  err = mt_http_client_post_request(client, module_http->token, post_data);
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
    if (content_size != module_http->response_content_size)
    {
      ESP_LOGE(TAG, "%4d content_size %d != response_content_size %d", __LINE__,
               content_size, module_http->response_content_size);
      err = ESP_ERR_HTTP_BASE;
      goto EXIT;
    }
    else
    {
      objs_out = mt_module_http_utils_parse_objects_res(
          module_http->response_content, objs_num);
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

static void mt_module_http_task_loop(mt_module_http_t *module_http)
{
  int issue_module_token_interval = 30 * 1000; // 30ms
  int show_module_retry_max = 10;
  int show_module_retry_count = 10;
  int show_module_interval = 30 * 1000; // 30ms
  int heartbeat_max = 6;
  int heartbeat_count = 6;
  int heartbeat_interval = 30 * 1000; // 30ms
  esp_err_t err = ESP_OK;
  module_t *module = NULL;

RESTART:

  while (true)
  {
    err = mt_module_http_actions_issue_module_token(module_http);
    if (err != ESP_OK)
    {
      ESP_LOGE(TAG, "%4d %s mt_module_http_actions_issue_module_token failed",
               __LINE__, __func__);
    }
    else
    {
      ESP_LOGI(TAG, "%4d %s mt_module_http_actions_issue_module_token success",
               __LINE__, __func__);
      break;
    }

    vTaskDelay(issue_module_token_interval / portTICK_PERIOD_MS);
  }

  show_module_retry_count = show_module_retry_max;

  while (true)
  {
    if (module != NULL)
    {
      mt_module_http_utils_free_module(module);
    }

    if (show_module_retry_count <= 0)
    {
      ESP_LOGE(TAG, "%4d %s show_module_retry_count get limit, restart loop",
               __LINE__, __func__);
      goto RESTART;
    }

    module = mt_module_http_actions_show_module(module_http);
    if (module == NULL)
    {
      ESP_LOGE(TAG, "%4d %s mt_module_http_actions_show_module failed",
               __LINE__, __func__);
      show_module_retry_count--;
    }
    else
    {
      ESP_LOGI(TAG, "%4d %s mt_module_http_actions_show_module success",
               __LINE__, __func__);
      if (module_http->module->id != NULL)
      {
        free(module_http->module->id);
      }
      module_http->module->id = malloc(strlen(module->id) + 1);
      module_http->module->id[strlen(module->id)] = '\0';
      memcpy(module_http->module->id, module->id, strlen(module->id));
      ESP_LOGW(TAG, "%4d %s mt_module_http_utils_free_module",
               __LINE__, __func__);
      mt_module_http_utils_free_module(module);
      ESP_LOGW(TAG, "%4d %s mt_module_http_utils_free_module success",
               __LINE__, __func__);
      break;
    }
    vTaskDelay(show_module_interval / portTICK_PERIOD_MS);
  }

  heartbeat_count = heartbeat_max;
  module_http->session_id =
      mt_utils_session_new_session(mt_utils_session_gen_startup_session(),
                                   mt_utils_session_gen_major_session());

  //debug here
  module_http->session_id = 12345678;
  while (true)
  {
    if (heartbeat_count <= 0)
    {
      ESP_LOGE(TAG, "%4d %s heartbeat_count get limit, restart loop", __LINE__,
               __func__);
      goto RESTART;
    }

    err = mt_module_http_actions_heartbeat(module_http, module_http->module);
    if (err != ESP_OK)
    {
      ESP_LOGE(TAG, "%4d %s mt_module_http_actions_heartbeat failed", __LINE__,
               __func__);
      heartbeat_count--;
    }
    else
    {
      ESP_LOGI(TAG, "%4d %s mt_module_http_actions_heartbeat success", __LINE__,
               __func__);
      heartbeat_count = heartbeat_max;
    }

    vTaskDelay(heartbeat_interval / portTICK_PERIOD_MS);
  }

  return;
}

void mt_module_http_task(mt_module_http_t *module_http, char *task_name)
{
  // arg check
  if (module_http == NULL)
  {
    ESP_LOGE(TAG, "%4d %s module_http NULL", __LINE__, __func__);
    return;
  }
  else
  {
    if (module_http->host == NULL)
    {
      ESP_LOGE(TAG, "%4d %s module_http->host NULL", __LINE__, __func__);
      return;
    }

    if (module_http->port == 0)
    {
      ESP_LOGE(TAG, "%4d %s module_http->host NULL", __LINE__, __func__);
      return;
    }

    if (module_http->cred == NULL)
    {
      ESP_LOGE(TAG, "%4d %s module_http->cred NULL", __LINE__, __func__);
      return;
    }
    else
    {
      if (module_http->cred->id == NULL)
      {
        ESP_LOGE(TAG, "%4d %s module_http->cred->id NULL", __LINE__, __func__);
        return;
      }

      if (module_http->cred->secret == NULL)
      {
        ESP_LOGE(TAG, "%4d %s module_http->cred->secret NULL", __LINE__,
                 __func__);
        return;
      }
    }

    if (module_http->event_handler == NULL)
    {
      ESP_LOGE(TAG, "%4d %s module_http->event_handle  NULL", __LINE__,
               __func__);
      return;
    }
  }

  xTaskCreate((TaskFunction_t)mt_module_http_task_loop, task_name, 8 * 1024,
              module_http, 10, NULL);
}
