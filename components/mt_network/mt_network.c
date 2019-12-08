#include "esp_log.h"
#include "string.h"

#include "mt_network.h"
#include "mt_nvs_config.h"

// global define ==============================================================

static const char *TAG = "MT_NETWORK";

// global func ================================================================

esp_err_t mt_network_task(int light_pin, int light_pin_on_level, int btn_pin,
                          int btn_pin_on_level) {
  esp_err_t err = ESP_OK;
  mt_nvs_host_t *host_info = malloc(sizeof(mt_nvs_host_t));

  err = mt_nvs_config_get_host_config(host_info);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_get_host_config failed", __LINE__,
             __func__);
    return err;
  }

  int match = 0;
  if (strcmp(host_info->net_type, "wifi") == 0) {
    mt_smartconfig_task(light_pin, light_pin_on_level, btn_pin,
                        btn_pin_on_level);
    match = 1;
  }

  if (strcmp(host_info->net_type, "eth") == 0) {
    mt_ethernet_task(light_pin, light_pin_on_level);
    match = 1;
  }

  if (match == 0) {
    ESP_LOGE(TAG, "%4d %s unexcept net type:%s", __LINE__, __func__,
             host_info->net_type);
  }

  return ESP_OK;
}
