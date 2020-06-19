#include "modbus_power001_storage.h"

#include "freertos/FreeRTOS.h"

#include "modbus_power001_lora_flow.h"
#include "mt_nvs_storage.h"
#include "rs232_lora_ebyte.h"

// static define ==============================================================

static const char *TAG = "MODBUS_POWER001_STORAGE";

// help func ================================================================

modbus_power001_config_t *modbus_power001_storage_new_config(int size) {
  if (size <= 0) {
    ESP_LOGE(TAG, "%4d %s size error:%d", __LINE__, __func__, size);
    return NULL;
  }

  modbus_power001_config_t *config = malloc(sizeof(modbus_power001_config_t));

  config->power_num = size;
  config->power_addr = malloc(size * sizeof(int));
  for (int i = 0; i < size; i++) {
    config->power_addr[i] = 0;
  }

  return config;
}

void modbus_power001_storage_free_config(modbus_power001_config_t *config) {
  if (config == NULL)
    return;

  if (config->power_num == 0) {
    free(config);
    return;
  } else {
    free(config->power_addr);
  }

  free(config);
}

esp_err_t
modbus_power001_storage_check_config(modbus_power001_config_t *config) {
  if (config == NULL) {
    ESP_LOGE(TAG, "%4d %s config NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (config->power_num <= 0) {
    ESP_LOGE(TAG, "%4d %s config->power_num:%d error", __LINE__, __func__,
             config->power_num);
    return ESP_ERR_INVALID_ARG;
  }

  if (config->power_addr == NULL) {
    ESP_LOGE(TAG, "%4d %s config->power_addr NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}

// global func ================================================================

modbus_power001_config_t *modbus_power001_storage_get_config() {
  esp_err_t err = ESP_OK;
  int power_num = 0;
  char key[24] = "";

  if (mt_nvs_read_int32_config("power_num", &power_num) == false) {
    ESP_LOGE(TAG, "%4d %s read power_num failed", __LINE__, __func__);
    return NULL;
  }

  if (power_num <= 0) {
    ESP_LOGE(TAG, "%4d %s unexcept power_num:%d failed", __LINE__, __func__,
             power_num);
    return NULL;
  }

  modbus_power001_config_t *config =
      modbus_power001_storage_new_config(power_num);
  if (config == NULL) {
    ESP_LOGE(TAG, "%4d %s modbus_power001_storage_new_config failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  for (int i = 0; i < power_num; i++) {
    sprintf(key, "power_%d_addr", i + 1);
    if (mt_nvs_read_int32_config(key, &config->power_addr[i]) == false) {
      ESP_LOGE(TAG, "%4d %s read power_%d_addr failed", __LINE__, __func__,
               i + 1);
      err = ESP_ERR_INVALID_RESPONSE;
      goto EXIT;
    }
  }

EXIT:
  if (err != ESP_OK) {
    modbus_power001_storage_free_config(config);
    config = NULL;
  }
  return config;
}

esp_err_t modbus_power001_storage_set_config(modbus_power001_config_t *config) {
  char key[24] = "";
  esp_err_t err = ESP_OK;

  err = modbus_power001_storage_check_config(config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_power001_storage_check_config failed",
             __LINE__, __func__);
    return err;
  }

  if (mt_nvs_write_int32_config("power_num", config->power_num) == false) {
    ESP_LOGE(TAG, "%4d %s write power_num failed", __LINE__, __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  for (int i = 0; i < config->power_num; i++) {
    sprintf(key, "power_%d_addr", i);
    if (mt_nvs_write_int32_config(key, config->power_addr[i]) == false) {
      ESP_LOGE(TAG, "%4d %s write power_%d_addr failed", __LINE__, __func__, i);
      return ESP_ERR_INVALID_RESPONSE;
    }
  }

  modbus_power001_updata_config(config);

  return ESP_OK;
}

esp_err_t modbus_power001_storage_get_interval(int *interval) {
  if (mt_nvs_read_int32_config("power_interval", interval) == false) {
    ESP_LOGE(TAG, "%4d %s read interval failed", __LINE__, __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

esp_err_t modbus_power001_storage_set_interval(int interval) {
  if (mt_nvs_write_int32_config("power_interval", interval) == false) {
    ESP_LOGE(TAG, "%4d %s write interval failed", __LINE__, __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  modbus_power001_update_interval(interval);

  return ESP_OK;
}
