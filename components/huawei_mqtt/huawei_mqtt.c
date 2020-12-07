#include "string.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "mt_mqtt_lan.h"

#include "huawei_mqtt.h"
#include "mt_nvs_config.h"

// global value ===============================================================

static const char *TAG = "HUAWEI_MQTT";

static esp_mqtt_client_handle_t Mqtt_Client;
static char TOPIC_HEAD[128] = "";
void (*msg_process)(char *topic, void *buf, int size);

// static func ================================================================

static esp_err_t message_arrived_callback(esp_mqtt_event_handle_t event) {
  esp_err_t err = ESP_OK;
  char topic[128] = "";

  esp_mqtt_client_handle_t client = event->client;
  switch (event->event_id) {
  case MQTT_EVENT_CONNECTED: {
    ESP_LOGI(TAG, "mqtt connect");
    strcpy(topic, TOPIC_HEAD);
    strcat(topic, "/command");
    err = esp_mqtt_client_subscribe(client, topic, 0);
    if (err == -1) {
      ESP_LOGE(TAG, "%d esp_mqtt_client_subscribe error", __LINE__);
      esp_mqtt_client_reconnect(client);
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
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  EXIT:
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

esp_err_t huawei_mqtt_pub_msg(char *subtopic, char *buf, int size) {
  esp_err_t err = ESP_OK;
  char topic[256] = "";

  strcpy(topic, TOPIC_HEAD);
  strcpy(topic + strlen(TOPIC_HEAD), subtopic);

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

esp_err_t huawei_mqtt_init(void (*handle)(char *topic, void *buf, int size)) {
  esp_err_t err = ESP_OK;
  mt_nvs_host_t *host = NULL;

  msg_process = handle;

  host = mt_nvs_config_get_host_config();
  if (host == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_get_host_config NULL", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (strcmp(host->platform, HUAWEI_PLATFORM) != 0) {
    ESP_LOGE(TAG, "%4d %s config platform:%s expect platformL:%s", __LINE__,
             __func__, host->platform, HUAWEI_PLATFORM);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  strcpy(TOPIC_HEAD, host->topicHead);

  char mqtt_uri_str[128] = "";
  strcpy(mqtt_uri_str, "mqtt://");
  strcat(mqtt_uri_str, host->host);
  strcat(mqtt_uri_str, ":");
  strcat(mqtt_uri_str, host->mqtt_port);

  const esp_mqtt_client_config_t mqtt_cfg = {
      .uri = mqtt_uri_str,
      .username = host->username,
      .password = host->password,
      .client_id = host->clientID,
      .event_handle = (mqtt_event_callback_t)message_arrived_callback,
  };

  Mqtt_Client = esp_mqtt_client_init(&mqtt_cfg);

  printf("uri:%s username:%s password:%s clientID:%s topic_head:%s\n",
         mqtt_uri_str, host->username, host->password, host->clientID,
         TOPIC_HEAD);

  err = esp_mqtt_client_start(Mqtt_Client);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%d esp_mqtt_client_start failed", __LINE__);
    return ESP_ERR_INVALID_STATE;
  }

EXIT:
  mt_nvs_config_free_host(host);
  return err;
}
