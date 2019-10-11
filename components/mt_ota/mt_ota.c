#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "mt_ota.h"

// global define ==============================================================
static const char *TAG = "MT_OTA";
static bool mt_ota_state = false; // use to make one ota task

// static func ================================================================
static esp_err_t _mt_ota_event_handler(esp_http_client_event_t *evt)
{
  switch (evt->event_id)
  {
  case HTTP_EVENT_ERROR:
    ESP_LOGD(TAG, "%4d %s HTTP_EVENT_ERROR", __LINE__, __func__);
    break;
  case HTTP_EVENT_ON_CONNECTED:
    ESP_LOGD(TAG, "%4d %s HTTP_EVENT_ON_CONNECTED", __LINE__, __func__);
    break;
  case HTTP_EVENT_HEADER_SENT:
    ESP_LOGD(TAG, "%4d %s HTTP_EVENT_HEADER_SENT", __LINE__, __func__);
    break;
  case HTTP_EVENT_ON_HEADER:
    ESP_LOGD(TAG, "%4d %s HTTP_EVENT_ON_HEADER, key=%s, value=%s", __LINE__,
             __func__, evt->header_key, evt->header_value);
    break;
  case HTTP_EVENT_ON_DATA:
    ESP_LOGD(TAG, "%4d %s HTTP_EVENT_ON_DATA, len=%d", __LINE__, __func__,
             evt->data_len);
    break;
  case HTTP_EVENT_ON_FINISH:
    ESP_LOGD(TAG, "%4d %s HTTP_EVENT_ON_FINISH", __LINE__, __func__);
    break;
  case HTTP_EVENT_DISCONNECTED:
    ESP_LOGD(TAG, "%4d %s HTTP_EVENT_DISCONNECTED", __LINE__, __func__);
    break;
  }
  return ESP_OK;
}

// global func ================================================================
static void mt_ota_loop(mt_ota_t *ota)
{
  esp_http_client_config_t config = {
      .url = ota->url,
      .event_handler = _mt_ota_event_handler,
  };

  ESP_LOGI(TAG, "%4d %s OTA begin, url=%s", __LINE__, __func__, ota->url);

  for (int i = 0; i < 3; i++)
  {
    esp_err_t ret = esp_https_ota(&config);
    if (ret != ESP_OK)
    {
      ESP_LOGE(TAG, "%4d %s OTA %d failed", __LINE__, __func__, i + 1);
    }
    else
    {
      ESP_LOGI(TAG, "%4d %s OTA success, reboot now", __LINE__, __func__);
      esp_restart();
      goto EXIT;
    }

    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }

EXIT:
  mt_ota_state = false;
  vTaskDelete(NULL);
}

mt_ota_t *mt_ota_default_new(char *url)
{
  mt_ota_t *ota = malloc(sizeof(mt_ota_t));

  ota->url = url;

  return ota;
}

esp_err_t mt_ota_task(mt_ota_t *ota)
{
  if (mt_ota_state == true)
  {
    return ESP_ERR_INVALID_RESPONSE;
  }

  mt_ota_state = true;
  xTaskCreate((TaskFunction_t)mt_ota_loop, "MT_OTA", 8 * 1024, ota, 5, NULL);

  return ESP_OK;
}