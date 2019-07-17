#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "esp_event_loop.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "esp_wifi.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"

#include "mt_mqtt.h"
#include "mt_nvs_storage.h"
#include "mt_utils.h"

static const char *TAG = "MT_MQTT_LAN_EXAMPLE";

#define WIFI_SSID "Nayotta"
#define WIFI_PASS "pink_man"
#define MQTT_HOST "10.1.1.130"
#define MQTT_PORT "1883"
#define MQTT_CRED "d692d6c9322b4c7b8f416f51b04a2270"
#define MQTT_SKEY "0WXViH7SYRJWaHJfJbmiite/uDncJ642pxrXv8LIwKU="
#define MQTT_MODULE_ID "86f63fff1bb64d62a8c9c30aa8e00f31"
#define MQTT_USE_LAN

static EventGroupHandle_t wifi_event_group;
const static int CONNECTED_BIT = BIT0;

static esp_err_t wifi_event_handler(void *ctx, system_event_t *event) {
  switch (event->event_id) {
    case SYSTEM_EVENT_STA_START:
      esp_wifi_connect();
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);

      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      esp_wifi_connect();
      xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
      break;
    default:
      break;
  }
  return ESP_OK;
}

static void wifi_init(void) {
  tcpip_adapter_init();
  wifi_event_group = xEventGroupCreate();
  ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  wifi_config_t wifi_config = {
      .sta =
          {
              .ssid = WIFI_SSID,
              .password = WIFI_PASS,
          },
  };
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
  ESP_LOGI(TAG, "start the WIFI SSID:[%s]", WIFI_SSID);
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_LOGI(TAG, "Waiting for wifi");
  xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true,
                      portMAX_DELAY);
}

void mt_mqtt_handle(void *buf, int size) {
  ESP_LOGI(TAG, "%s %d get mqtt msg", __func__, __LINE__);
}

void app_main() {
  bool ret = false;
  unsigned char *hmac_str = NULL;

  ESP_LOGI(TAG, "test begin");

  ret = mt_nvs_init();
  if (ret == false) {
    ESP_LOGE(TAG, "%d mt_nvs_init failed", __LINE__);
    return;
  }

  wifi_init();

  hmac_str = mt_hmac_sha256_mqtt((uint8_t *)MQTT_SKEY, sizeof(MQTT_SKEY) - 1,
                                 (uint8_t *)MQTT_CRED, sizeof(MQTT_CRED) - 1);
  ESP_LOGW(TAG, "deviceid=%s", MQTT_MODULE_ID);
  ESP_LOGW(TAG, "username=%s", MQTT_CRED);
  ESP_LOGW(TAG, "password=%s", hmac_str);

  ret = mt_mqtt_init(MQTT_HOST, MQTT_PORT, MQTT_CRED, (char *)hmac_str,
                     MQTT_MODULE_ID, mt_mqtt_handle);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "%s %d mqtt_init failed", __func__, __LINE__);
    return;
  }

  mt_mqtt_task();

  while (1) {
    uint8_t *buf = (uint8_t *)"test";
    int buf_size = 4;
    mt_mqtt_pub_msg(
        "mt/"
        "86f63fff1bb64d62a8c9c30aa8e00f3186f63fff1bb64d62a8c9c30aa8e00f3186f63f"
        "ff1bb64d62a8c9c30aa8e00f3186f63fff1bb64d62a8c9c30aa8e00f3186f63fff1bb6"
        "4d62a8c9c30aa8e00f3186f63fff1bb64d62a8c9c30aa8e00f3186f63fff1bb64d62a8"
        "c9c30aa8e00f3186f63fff1bb64d62a8c9c30aa8e00f3186f63fff1bb64d62a8c9c30a"
        "a8e00f3186f63fff1bb64d62a8c9c30aa8e00f3186f63fff1bb64d62a8c9c30aa8e00f"
        "31/down", buf, buf_size);

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    
  }

  ESP_LOGI(TAG, "test end");
}
