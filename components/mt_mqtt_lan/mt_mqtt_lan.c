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

#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "mqtt_client.h"
#include "nvs_flash.h"

#include "mt_mqtt.h"
#include "mt_mqtt_lan.h"
#include "mt_nvs_config.h"
#include "mt_utils.h"

// global value ===============================================================
static const char *TAG = "MT_MQTT_LAN";

esp_mqtt_client_handle_t Mqtt_Client;
extern char Module_id[128];
extern char Device_id[128];
extern uint64_t Session_id;
void (*msg_process)(char *topic, void *buf, int size);

// static func ================================================================
static esp_err_t message_arrived_callback(esp_mqtt_event_handle_t event) {
  esp_err_t err;

  esp_mqtt_client_handle_t client = event->client;
  switch (event->event_id) {
  case MQTT_EVENT_CONNECTED: {
    ESP_LOGI(TAG, "mqtt connect");
    char topic[TOPIC_MAX_SIZE] = "";
    sprintf(topic, "mt/modules/%s/+/sessions/+/downstream", Module_id);

    ESP_LOGI(TAG, "%d sub %s", __LINE__, topic);
    err = esp_mqtt_client_subscribe(client, topic, 0);
    if (err == -1) {
      ESP_LOGE(TAG, "%d esp_mqtt_client_subscribe error", __LINE__);
    }

    if (strcmp(Device_id, "") != 0) {
      char topic[TOPIC_MAX_SIZE] = "";
      sprintf(topic, "mt/devices/%s/+/sessions/+/downstream", Device_id);

      ESP_LOGI(TAG, "%d sub %s", __LINE__, topic);
      err = esp_mqtt_client_subscribe(client, topic, 0);
      if (err == -1) {
        ESP_LOGE(TAG, "%d esp_mqtt_client_subscribe error", __LINE__);
      }
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
    char *topic = malloc(event->topic_len + 1);
    memcpy(topic, event->topic, event->topic_len);
    topic[event->topic_len] = '\0';
    ESP_LOGI(TAG, "%d get data, topic:%s size=%d", __LINE__, topic,
             (int)event->data_len);
    if (msg_process != NULL) {
      (*msg_process)(topic, (void *)event->data, (int)event->data_len);
    } else {
      ESP_LOGE(TAG, "%d msg_process not init", __LINE__);
      return ESP_ERR_INVALID_ARG;
    }
    free(topic);
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
esp_err_t mqtt_pub_msg(char *topic, uint8_t *buf, int size) {
  esp_err_t err;

  ESP_LOGI(TAG, "%4d %s mqtt pub msg,topic=%s,size=%d", __LINE__, __func__,
           topic, size);
  err = esp_mqtt_client_publish(Mqtt_Client, topic, (const char *)buf, size, 0,
                                0);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%d %s error", __LINE__, __func__);
    return ESP_ERR_INVALID_STATE;
  }

  return err;
}

esp_err_t mqtt_init(int mod_index, char *module_id, uint64_t session_id,
                    char *device_id,
                    void (*handle)(char *topic, void *buf, int size)) {

  // char *host, char *port, char *username, char *key,
  esp_err_t err;
  unsigned char *hmac_str = NULL;
  mt_nvs_host_t *host = NULL;
  mt_nvs_module_t *mod = NULL;

  host = mt_nvs_config_get_host_config();
  if (host == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_get_host_config failed", __LINE__,
             __func__);
    return ESP_ERR_INVALID_ARG;
  }

  mod = mt_nvs_config_get_module(mod_index);
  if (mod == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_get_module index:%d failed", __LINE__,
             __func__, mod_index);
    return ESP_ERR_INVALID_ARG;
  }

  char mqtt_uri_str[URI_MAX_SIZE] = "";
  strcpy(mqtt_uri_str, "mqtt://");
  strcat(mqtt_uri_str, host->host);
  strcat(mqtt_uri_str, ":");
  strcat(mqtt_uri_str, host->mqtt_port);

  msg_process = handle;

  hmac_str = mt_hmac_sha256_mqtt((uint8_t *)mod->key, strlen(mod->key),
                                 (uint8_t *)mod->id, strlen(mod->id));

  const esp_mqtt_client_config_t mqtt_cfg = {
      .uri = mqtt_uri_str,
      .username = mod->id,
      .password = (char *)hmac_str,
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

esp_err_t mqtt_init_origin(char *username, char *password, char *clientID,
                           void (*handle)(char *topic, void *buf, int size)) {
  esp_err_t err;
  mt_nvs_host_t *host = NULL;

  host = mt_nvs_config_get_host_config();
  if (host == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_get_host_config failed", __LINE__,
             __func__);
    return ESP_ERR_INVALID_ARG;
  }

  char mqtt_uri_str[URI_MAX_SIZE] = "";
  strcpy(mqtt_uri_str, "mqtt://");
  strcat(mqtt_uri_str, host->host);
  strcat(mqtt_uri_str, ":");
  strcat(mqtt_uri_str, host->mqtt_port);

  msg_process = handle;

  const esp_mqtt_client_config_t mqtt_cfg = {
      .uri = mqtt_uri_str,
      .username = username,
      .password = password,
      .client_id = clientID,
      .event_handle = (mqtt_event_callback_t)message_arrived_callback,
  };

  Mqtt_Client = esp_mqtt_client_init(&mqtt_cfg);

  err = esp_mqtt_client_start(Mqtt_Client);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%d esp_mqtt_client_start failed", __LINE__);
    return ESP_ERR_INVALID_STATE;
  }

  //mt_nvs_config_free_host(host);

  return ESP_OK;
}

esp_err_t mt_mqtt_lan_update_session_id(uint64_t session_id) {
  Session_id = session_id;

  return ESP_OK;
}
