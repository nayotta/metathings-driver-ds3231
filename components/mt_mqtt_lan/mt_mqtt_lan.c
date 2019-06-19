#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"

#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "mqtt_client.h"
#include "nvs_flash.h"

#include "mt_mqtt.h"
#include "mt_mqtt_lan.h"

// global value ===============================================================
static const char *TAG = "NYT_MQTT_LAN";

esp_mqtt_client_handle_t Mqtt_Client;
char Device_Id[128] = "";
void (*msg_process)(void *buf, int size);

// static func ================================================================
static esp_err_t message_arrived_callback(esp_mqtt_event_handle_t event) {
  esp_err_t err;

  esp_mqtt_client_handle_t client = event->client;
  switch (event->event_id) {
    case MQTT_EVENT_CONNECTED: {
      ESP_LOGI(TAG, "mqtt connect");
      char topic[TOPIC_MAX_SIZE] = "";

      strcpy(topic, "mt/");
      strcat(topic, Device_Id);
      strcat(topic, "/down");

      ESP_LOGI(TAG, "%d sub %s", __LINE__, topic);
      err = esp_mqtt_client_subscribe(client, topic, 0);
      if (err == -1) {
        ESP_LOGE(TAG, "%d esp_mqtt_client_subscribe error", __LINE__);
      }

      break;
    }
    case MQTT_EVENT_DISCONNECTED:
      ESP_LOGW(TAG, "%d mqtt disconnect", __LINE__);
      // (TODO zh) disconnected process
      break;
    case MQTT_EVENT_SUBSCRIBED:
      ESP_LOGI(TAG, "%d mqtt sub success", __LINE__);
      break;
    case MQTT_EVENT_UNSUBSCRIBED:
      ESP_LOGI(TAG, "%d mqtt unsub success", __LINE__);
      break;
    case MQTT_EVENT_PUBLISHED:
      ESP_LOGI(TAG, "%d mqtt pub", __LINE__);
      break;
    case MQTT_EVENT_DATA: {
      ESP_LOGI(TAG, "%d get data", __LINE__);
      if (msg_process != NULL) {
        (*msg_process)((void *)event->data, (int)event->data_len);
      } else {
        ESP_LOGE(TAG, "%d msg_process not init", __LINE__);
        return ESP_ERR_INVALID_ARG;
      }
      break;
    }
    case MQTT_EVENT_ERROR:
      ESP_LOGE(TAG, "%d mqtt error event", __LINE__);
      // (TODO zh)
      break;
    default:
      break;
  }
  return ESP_OK;
}

// global func ================================================================
int mqtt_pub_msg(char *topic, uint8_t *buf, int size) {
  esp_err_t err;

  err = esp_mqtt_client_publish(Mqtt_Client, topic, (const char *)buf, size, 0,
                                0);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%d error", __LINE__);
    return ESP_ERR_INVALID_STATE;
  }

  return ESP_OK;
}

int mqtt_init(char *host, char *port, char *username, char *password,
              char *module_id, void (*handle)(void *buf, int size)) {
  esp_err_t err;

  char mqtt_uri_str[URI_MAX_SIZE] = "";
  strcpy(mqtt_uri_str, "mqtt://");
  strcat(mqtt_uri_str, host);
  strcat(mqtt_uri_str, ":");
  strcat(mqtt_uri_str, port);

  strcpy(Device_Id, module_id);
  msg_process = handle;

  const esp_mqtt_client_config_t mqtt_cfg = {
      .uri = mqtt_uri_str,
      .username = username,
      .password = password,
      .event_handle = (mqtt_event_callback_t)message_arrived_callback,
  };

  Mqtt_Client = esp_mqtt_client_init(&mqtt_cfg);

  err = esp_mqtt_client_start(Mqtt_Client);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%d esp_mqtt_client_start failed", __LINE__);
    return ESP_ERR_INVALID_STATE;
  }

  return ESP_OK;
}