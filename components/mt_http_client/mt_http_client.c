
#include "esp_http_client.h"
#include "esp_log.h"
#include "string.h"

#include "freertos/semphr.h"

#include "mt_http_client.h"

// global define ==============================================================
static const char *TAG = "MT_HTTP_CLIENT";
static SemaphoreHandle_t SemaphorMasterHdl = NULL;
static int32_t LOCK_TIMEOUT = 3000;

// static func ================================================================
static void mt_http_lock_init() {
  if (SemaphorMasterHdl == NULL) {
    SemaphorMasterHdl = xSemaphoreCreateMutex();
  }
  return;
}

static bool mt_http_lock_take(int32_t timeout) {
  if (xSemaphoreTake(SemaphorMasterHdl, (portTickType)timeout) == pdTRUE) {
    return true;
  }
  return false;
}

static void mt_http_lock_release() {
  xSemaphoreGive(SemaphorMasterHdl);
  return;
}

// global func ================================================================
esp_err_t mt_http_client_post_request(esp_http_client_handle_t client,
                                      char *token, char *post_data) {
  esp_err_t err;

  // lock
  mt_http_lock_init();
  if (mt_http_lock_take(LOCK_TIMEOUT) == false) {
    ESP_LOGE(TAG, "%4d %s mt_http_lock_take timeout", __LINE__, __func__);
    return ESP_ERR_TIMEOUT;
  }

  // set method post
  err = esp_http_client_set_method(client, HTTP_METHOD_POST);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d esp_http_client_set_method failed code=%d", __LINE__,
             err);
    return err;
  }

  // set auth head
  if (token != NULL) {
    char token_bearer[128];
    strcpy(token_bearer, "Bearer ");
    strcat(token_bearer, token);

    // ESP_LOGW(TAG, "%4d %s get token:%s", __LINE__, __func__, token_bearer);

    err = esp_http_client_set_header(client, "Authorization", token_bearer);
    if (err != ESP_OK) {
      ESP_LOGE(TAG,
               "%4d esp_http_client_set_header authorization failed code=%d",
               __LINE__, err);
      goto EXIT;
    }
  }

  // set accept type
  err = esp_http_client_set_header(client, "accept", "application/json");
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d esp_http_client_set_header accept failed code=%d",
             __LINE__, err);
    goto EXIT;
  }

  // set post_data and content_type
  if (post_data != NULL) {
    err =
        esp_http_client_set_header(client, "Content-Type", "application/json");
    if (err != ESP_OK) {
      ESP_LOGE(TAG,
               "%4d esp_http_client_set_header Content-Type failed code=%d",
               __LINE__, err);
      goto EXIT;
    }

    err = esp_http_client_set_post_field(client, (char *)post_data,
                                         strlen(post_data));
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d esp_http_client_set_post_field failed code=%d",
               __LINE__, err);
      goto EXIT;
    }
  } else {
    // do not check ret may be return no-exist error
    esp_http_client_set_post_field(client, NULL, 0);
  }

  // sync request
  err = esp_http_client_perform(client);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d esp_http_client_perform failed code:%d", __LINE__, err);
    goto EXIT;
  }
EXIT:
  mt_http_lock_release();
  return err;
}
