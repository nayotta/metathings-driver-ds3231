
#include "esp_http_client.h"
#include "esp_log.h"
#include "string.h"

#include "mt_http_client.h"

// global define ==============================================================
static const char *TAG = "MT_HTTP";

// global func ================================================================
esp_err_t mt_http_client_post_request(esp_http_client_handle_t client,
                                      char *token, char *post_data)
{
  esp_err_t err;

  // set method post
  err = esp_http_client_set_method(client, HTTP_METHOD_POST);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%d esp_http_client_set_method failed", __LINE__);
    return err;
  }

  // set auth head
  if (token != NULL)
  {
    err = esp_http_client_set_header(client, "authorization", token);
    if (err != ESP_OK)
    {
      ESP_LOGE(TAG, "%d esp_http_client_set_header authorization failed", __LINE__);
      return err;
    }
  }

  // set accept type
  err = esp_http_client_set_header(client, "accept", "application/json");
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%d esp_http_client_set_header accept failed", __LINE__);
    return err;
  }

  // set post_data and content_type
  if (post_data != NULL)
  {
    err =
        esp_http_client_set_header(client, "Content-Type", "application/json");
    if (err != ESP_OK)
    {
      ESP_LOGE(TAG, "%d esp_http_client_set_header Content-Type failed", __LINE__);
      return err;
    }

    err = esp_http_client_set_post_field(client, (char *)post_data,
                                         strlen(post_data));
    if (err != ESP_OK)
    {
      ESP_LOGE(TAG, "%d esp_http_client_set_post_field failed", __LINE__);
      return err;
    }
  }
  else
  {
    err = esp_http_client_set_header(client, "Content-Type", NULL);
    if (err != ESP_OK)
    {
      ESP_LOGE(TAG, "%d esp_http_client_set_header Content-Type failed", __LINE__);
      return err;
    }

    err = esp_http_client_set_post_field(client, NULL, 0);
    if (err != ESP_OK)
    {
      ESP_LOGE(TAG, "%d esp_http_client_set_post_field failed", __LINE__);
      return err;
    }
  }

  //sync request
  err = esp_http_client_perform(client);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%d esp_http_client_perform failed code:%d", __LINE__, err);
    return err;
  }

  return ESP_OK;
}
