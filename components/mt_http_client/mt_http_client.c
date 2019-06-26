
#include "esp_http_client.h"
#include "esp_log.h"
#include "string.h"

#include "mt_http_client.h"

// global define ==============================================================
static const char *TAG = "MT_HTTP_CLIENT";

// global func ================================================================
esp_err_t mt_http_client_post_request(esp_http_client_handle_t client,
                                      char *token, char *post_data)
{
  esp_err_t err;

  // set method post
  err = esp_http_client_set_method(client, HTTP_METHOD_POST);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d esp_http_client_set_method failed code=%d", __LINE__, err);
    return err;
  }

  // set auth head
  if (token != NULL)
  {
    err = esp_http_client_set_header(client, "authorization", token);
    if (err != ESP_OK)
    {
      ESP_LOGE(TAG, "%4d esp_http_client_set_header authorization failed code=%d", __LINE__, err);
      return err;
    }
  }

  // set accept type
  err = esp_http_client_set_header(client, "accept", "application/json");
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d esp_http_client_set_header accept failed code=%d", __LINE__, err);
    return err;
  }

  // set post_data and content_type
  if (post_data != NULL)
  {
    err =
        esp_http_client_set_header(client, "Content-Type", "application/json");
    if (err != ESP_OK)
    {
      ESP_LOGE(TAG, "%4d esp_http_client_set_header Content-Type failed code=%d", __LINE__, err);
      return err;
    }

    err = esp_http_client_set_post_field(client, (char *)post_data,
                                         strlen(post_data));
    if (err != ESP_OK)
    {
      ESP_LOGE(TAG, "%4d esp_http_client_set_post_field failed code=%d", __LINE__, err);
      return err;
    }
  }
  else
  {
    // do not check ret may be return no-exist error
    esp_http_client_set_post_field(client, NULL, 0);
  }

  //sync request
  err = esp_http_client_perform(client);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d esp_http_client_perform failed code:%d", __LINE__, err);
    return err;
  }

  return ESP_OK;
}
