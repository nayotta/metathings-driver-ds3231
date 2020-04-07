#include "string.h"

#include "esp_log.h"

#include "mt_mqtt.h"
#include "mt_mqtt_lan.h"
#include "mt_nvs_config.h"
#include "rs232_sim_air720h.h"

// global value ===============================================================

static const char *TAG = "MT_MQTT";

// global func ================================================================

esp_err_t mt_mqtt_pub_msg(char *topic, uint8_t *buf, int size) {
  esp_err_t err = ESP_OK;
  char *net_type = NULL;
  int match = 0;

  net_type = mt_nvs_config_get_net_type();
  if (net_type == NULL) {
    ESP_LOGE(TAG, "%4d %s net_type NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  if (strstr(net_type, "lan")) {
    match++;
    err = mqtt_pub_msg(topic, buf, size);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
      err = ESP_ERR_INVALID_RESPONSE;
      goto EXIT;
    }
  }

  if (strstr(net_type, "air720h")) {
    match++;
    err = rs232_sim_air720h_mqtt_pub(topic, buf, size);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_mqtt_pub failed", __LINE__,
               __func__);
      err = ESP_ERR_INVALID_RESPONSE;
      goto EXIT;
    }
  }

  if (match <= 0) {
    ESP_LOGE(TAG, "%4d %s unmatch net_type:%s", __LINE__, __func__, net_type);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

EXIT:
  if (net_type != NULL)
    free(net_type);
  return err;
}

esp_err_t mt_mqtt_init(int mod_index, char *module_id, uint64_t session_id,
                       char *device_id,
                       void (*handle)(char *topic, void *buf, int size)) {
  esp_err_t err = ESP_OK;
  char *net_type = NULL;

  net_type = mt_nvs_config_get_net_type();
  if (net_type == NULL) {
    ESP_LOGE(TAG, "%4d %s net_type NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  if (strstr(net_type, "lan")) {
    err = mqtt_init(mod_index, module_id, session_id, device_id, handle);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%d  mqtt_init failed", __LINE__);
      goto EXIT;
    }
  }

  if (strstr(net_type, "air720h")) {
    err = rs232_sim_air720h_mqtt_init(mod_index, module_id, session_id,
                                      device_id, handle);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%d  rs232_sim_air720h_mqtt_init failed", __LINE__);
      goto EXIT;
    }
  }

EXIT:
  if (net_type != NULL)
    free(net_type);
  return err;
}
