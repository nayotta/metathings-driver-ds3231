#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/time.h"
#include "time.h"

#include "esp_log.h"
#include "esp_system.h"

#include "cJSON.h"

#include "mt_utils.h"
#include "mt_utils_login.h"

// global define ==============================================================
static const char *TAG = "MT_UTILS_LOGIN";

#define MT_UTILS_LOGIN_MAX_TIME_STR_SIZE 128
#define MT_UTILS_LOGIN_MAX_NONCE_STR_SIZE 32

// global func ================================================================
time_t mt_utils_login_get_time_now() {
  time_t now = 0;
  time(&now);

  return now;
}

char *mt_utils_login_time_to_ms_string(time_t time_in) {
  char temp_str[MT_UTILS_LOGIN_MAX_TIME_STR_SIZE] = "";
  char *time_str = NULL;

  sprintf(temp_str, "%ld000000000", time_in);
  time_str = malloc(strlen(temp_str) + 1);
  memcpy(time_str, temp_str, strlen(temp_str));
  time_str[strlen(temp_str)] = '\0';

  return time_str;
}

uint8_t *mt_utils_login_get_time_rfc3339nano_string(time_t time_in,
                                                    uint8_t *time_str_size) {
  struct tm timeinfo = {0};
  uint8_t *time_str = NULL;
  char temp_str[MT_UTILS_LOGIN_MAX_TIME_STR_SIZE] = "";

  localtime_r(&time_in, &timeinfo);

  sprintf(temp_str, "%d-%02d-%02dT%02d:%02d:%02d.000000000+00:00",
          timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
          timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  *time_str_size = strlen(temp_str);
  if (*time_str_size > MT_UTILS_LOGIN_MAX_TIME_STR_SIZE) {
    ESP_LOGE(TAG, "%3d %s time_str_size too long", __LINE__, __func__);
    goto EXIT;
  }
  time_str = malloc(*time_str_size + 1);
  memcpy(time_str, temp_str, *time_str_size);
  time_str[*time_str_size] = '\0';

EXIT:
  return time_str;
}

uint32_t mt_utils_login_get_nonce() { return esp_random(); }

char *mt_utils_login_get_issue_token_data(mt_module_http_t *module_http) {
  esp_err_t err = ESP_OK;
  char *data_out = NULL;
  cJSON *root = NULL;
  cJSON *cred_in_json = NULL;
  uint8_t *time_stamp = NULL;
  uint8_t time_stamp_size = 0;
  char *time_stamp_str = NULL;
  uint32_t nonce = 0;
  unsigned char *hmac = NULL;
  time_t now = 0;

  // check argument
  if (module_http == NULL) {
    ESP_LOGE(TAG, "%4d %s module_http is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  } else {
    if (module_http->cred == NULL) {
      ESP_LOGE(TAG, "%4d %s module_http->cred_id is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    } else {
      if (module_http->cred->id == NULL) {
        ESP_LOGE(TAG, "%4d %s module_http->cred->id is NULL", __LINE__,
                 __func__);
        err = ESP_ERR_INVALID_ARG;
        goto EXIT;
      }

      if (module_http->cred->secret == NULL) {
        ESP_LOGE(TAG, "%4d %s module_http->cred->secret is NULL", __LINE__,
                 __func__);
        err = ESP_ERR_INVALID_ARG;
        goto EXIT;
      }
    }
  }

  // count hmac
  now = mt_utils_login_get_time_now();
  time_stamp =
      mt_utils_login_get_time_rfc3339nano_string(now, &time_stamp_size);
  if (time_stamp == NULL) {
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
  if (hmac == NULL) {
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
  data_out = cJSON_Print(root);
  cJSON_Delete(root);

  //  ESP_LOGI(TAG, "%4d %s post_data =%s", __LINE__, __func__, data_out);

EXIT:
  if (time_stamp != NULL)
    free(time_stamp);

  if (time_stamp_str != NULL)
    free(time_stamp_str);

  if (hmac != NULL)
    free(hmac);

  if (err != ESP_OK) {
    data_out = NULL;
  }

  return data_out;
}

char *mt_utils_login_get_heartbeat_data(mt_module_http_t *module_http) {
  char *post_data = NULL;
  cJSON *root;

  root = cJSON_CreateObject();
  post_data = cJSON_Print(root);
  cJSON_Delete(root);

  return post_data;
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
