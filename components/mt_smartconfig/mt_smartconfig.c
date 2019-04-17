#include <stdlib.h>
#include <string.h>

#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_smartconfig.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "tcpip_adapter.h"

#include "gpio_light.h"
#include "mt_nvs_storage.h"
#include "mt_smartconfig.h"

static const char *TAG = "MT_SMARTCONFIG";

static mt_gpio_light_t *LIGHT_HANDLE = NULL;
EventGroupHandle_t WIFI_EVENT_GROUP;

const int CONNECTED_BIT = BIT0;
const int ESPTOUCH_DONE_BIT = BIT1;
const int WIFI_OK_BIT = BIT2;

// static func ================================================================
static void sc_callback(smartconfig_status_t status, void *pdata) {
  switch (status) {
    case SC_STATUS_WAIT:
      ESP_LOGI(TAG, "MT_SC_STATUS_WAIT");
      break;

    case SC_STATUS_FIND_CHANNEL:
      ESP_LOGI(TAG, "MT_SC_STATUS_FINDING_CHANNEL");
      break;

    case SC_STATUS_GETTING_SSID_PSWD:
      ESP_LOGI(TAG, "MT_SC_STATUS_GETTING_SSID_PSWD");
      break;

    case SC_STATUS_LINK: {
      bool ret = 0;
      int status = 0;

      ESP_LOGI(TAG, "MT_SC_STATUS_LINK");
      wifi_config_t *wifi_config = pdata;
      ESP_LOGI(TAG, "SSID:%s", wifi_config->sta.ssid);
      ESP_LOGI(TAG, "PASSWORD:%s", wifi_config->sta.password);

      // store config in nvs_storage
      ret = mt_nvs_write_string_config("password",
                                       (char *)wifi_config->sta.password);
      if (ret == false) {
        ESP_LOGE(TAG, "%d mt_nvs_write_string_config password failed",
                 __LINE__);
      }

      ret = mt_nvs_write_string_config("ssid", (char *)wifi_config->sta.ssid);
      if (ret == false) {
        ESP_LOGE(TAG, "%d mt_nvs_write_string_config ssid failed", __LINE__);
      }

      // config wifi
      status = esp_wifi_disconnect();
      if (status != ESP_OK) {
        ESP_LOGE(TAG, "%d esp_wifi_disconnect failed", __LINE__);
      }

      status = esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config);
      if (status != ESP_OK) {
        ESP_LOGE(TAG, "%d esp_wifi_set_config failed", __LINE__);
      }

      status = esp_wifi_connect();
      if (status != ESP_OK) {
        ESP_LOGE(TAG, "%d esp_wifi_connect failed", __LINE__);
      }

      break;
    }

    case SC_STATUS_LINK_OVER:
      ESP_LOGI(TAG, "MT_SC_STATUS_LINK_OVER");
      if (pdata != NULL) {
        uint8_t phone_ip[4] = {0};
        memcpy(phone_ip, (uint8_t *)pdata, 4);
        ESP_LOGI(TAG, "phone ip: %d.%d.%d.%d", phone_ip[0], phone_ip[1],
                 phone_ip[2], phone_ip[3]);
      }

      xEventGroupSetBits(WIFI_EVENT_GROUP, ESPTOUCH_DONE_BIT);
      xEventGroupSetBits(WIFI_EVENT_GROUP, WIFI_OK_BIT);
      break;

    default:
      ESP_LOGI(TAG, "smartconfig get unkown msg %d", status);
      break;
  }
}

static void mt_smartconfig_task_loop(void *parm) {
  int ret = 0;
  EventBits_t uxBits;

  ret = esp_smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "%d esp_smartconfig_set_type failed", __LINE__);
    goto EXIT;
  }

  ret = esp_smartconfig_fast_mode(1);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "%d esp_smartconfig_fast_mode failed", __LINE__);
    goto EXIT;
  }

  ret = esp_esptouch_set_timeout(240);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "%d esp_esptouch_set_timeout failed", __LINE__);
    goto EXIT;
  }

  ret = esp_smartconfig_start(sc_callback);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "%d esp_smartconfig_start failed", __LINE__);
    goto EXIT;
  }

  while (1) {
    ESP_LOGI(TAG, "WiFi Connecting");
    uxBits =
        xEventGroupWaitBits(WIFI_EVENT_GROUP, CONNECTED_BIT | ESPTOUCH_DONE_BIT,
                            true, false, portMAX_DELAY);
    if (uxBits & CONNECTED_BIT) {
      ESP_LOGI(TAG, "WiFi Connected to ap");
    }
    if (uxBits & ESPTOUCH_DONE_BIT) {
      ESP_LOGI(TAG, "smartconfig over");
      esp_smartconfig_stop();
      goto EXIT;
    }
  }

EXIT:
  vTaskDelete(NULL);
}

static esp_err_t event_handler(void *ctx, system_event_t *event) {
  switch (event->event_id) {
    case SYSTEM_EVENT_STA_START: {
      int ret = 0;
      char *ssid_str = NULL;
      size_t ssid_str_len = 0;
      char *password_str = NULL;
      size_t password_str_len = 0;

      // check nvs config save
      ssid_str = mt_nvs_read_string_config("ssid", &ssid_str_len);
      password_str = mt_nvs_read_string_config("password", &password_str_len);

      if (ssid_str_len > 1 && password_str_len > 1) {
        ESP_LOGI(TAG, "Find wifi store, read config from nvs");
        wifi_config_t wifi_config = {
            .sta =
                {
                    .ssid = "",
                    .password = "",
                },
        };

        if (LIGHT_HANDLE != NULL) {
          ret = mt_gpio_light_set_blink(LIGHT_HANDLE, 2000);
          if (ret == false) {
            ESP_LOGE(TAG, "%d mt_gpio_light_set_blink failed", __LINE__);
          }
        }

        strlcpy((char *)wifi_config.sta.ssid, ssid_str,
                sizeof(wifi_config.sta.ssid));
        strlcpy((char *)wifi_config.sta.password, password_str,
                sizeof(wifi_config.sta.password));

        ret = esp_wifi_disconnect();
        if (ret != ESP_OK) {
          ESP_LOGE(TAG, "%d esp_wifi_disconnect failed", __LINE__);
        }

        ret = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
        if (ret != ESP_OK) {
          ESP_LOGE(TAG, "%d esp_wifi_set_config failed", __LINE__);
        }

        ret = esp_wifi_connect();
        if (ret != ESP_OK) {
          ESP_LOGE(TAG, "%d esp_wifi_connect failed", __LINE__);
        }
      } else {
        bool ret1 = false;

        ESP_LOGI(TAG, "No wifi in nvs_storage, begin smartconfig");

        if (LIGHT_HANDLE != NULL) {
          ret1 = mt_gpio_light_set_blink(LIGHT_HANDLE, 150);
          if (ret1 == false) {
            ESP_LOGE(TAG, "%d mt_gpio_light_set_blink failed", __LINE__);
          }
        }

        xTaskCreate(mt_smartconfig_task_loop, "MT_SMARTCONFIG_TASK", 4096, NULL,
                    0, NULL);
      }
      break;
    }

    case SYSTEM_EVENT_STA_LOST_IP: {
      int ret = 0;

      ESP_LOGI(TAG, "[MT] sta ip lost");

      ret = esp_wifi_disconnect();
      if (ret != ESP_OK) {
        ESP_LOGE(TAG, "%d esp_wifi_disconnect failed", __LINE__);
      }

      ret = esp_wifi_start();
      if (ret != ESP_OK) {
        ESP_LOGE(TAG, "%d esp_wifi_start failed", __LINE__);
      }

      ESP_LOGI(TAG, "[MT] sta restart");
      break;
    }

    case SYSTEM_EVENT_STA_GOT_IP: {
      bool ret = false;

      xEventGroupSetBits(WIFI_EVENT_GROUP, CONNECTED_BIT);
      xEventGroupSetBits(WIFI_EVENT_GROUP, WIFI_OK_BIT);
      if (LIGHT_HANDLE != NULL) {
        ret = mt_gpio_light_set_on(LIGHT_HANDLE);
        if (ret == false) {
          ESP_LOGE(TAG, "%d mt_gpio_light_set_on failed", __LINE__);
        }
      }
      break;
    }

    case SYSTEM_EVENT_STA_DISCONNECTED: {
      bool ret = false;
      int ret1 = 0;

      ESP_LOGI(TAG, "[MT] sta disconnect");

      ret1 = esp_wifi_connect();
      if (ret1 != ESP_OK) {
        ESP_LOGE(TAG, "%d esp_wifi_connect failed", __LINE__);
      }

      if (LIGHT_HANDLE != NULL) {
        ret = mt_gpio_light_set_blink(LIGHT_HANDLE, 2000);
        if (ret == false) {
          ESP_LOGE(TAG, "%d mt_gpio_light_set_blink failed", __LINE__);
        }
      }

      xEventGroupClearBits(WIFI_EVENT_GROUP, CONNECTED_BIT);
      break;
    }

    default:
      ESP_LOGI(TAG, "STA get unkown id %d", event->event_id);
      break;
  }
  return ESP_OK;
}

// public func ================================================================
void mt_smartconfig_set_light_handle(mt_gpio_light_t *light_handle) {
  LIGHT_HANDLE = light_handle;
}

void mt_smartconfig_task(void) {
  int ret = 0;

  tcpip_adapter_init();

  WIFI_EVENT_GROUP = xEventGroupCreate();

  ret = esp_event_loop_init(event_handler, NULL);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "%d esp_event_loop_init failed", __LINE__);
  }

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

  ret = esp_wifi_init(&cfg);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "%d esp_wifi_init failed", __LINE__);
  }

  ret = esp_wifi_set_mode(WIFI_MODE_STA);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "%d esp_wifi_set_mode failed", __LINE__);
  }

  ret = esp_wifi_start();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "%d esp_wifi_start failed", __LINE__);
  }

  xEventGroupWaitBits(WIFI_EVENT_GROUP, CONNECTED_BIT, false, true,
                      portMAX_DELAY);

  ESP_LOGI(TAG, "mt_smartconfig_task finished");
}