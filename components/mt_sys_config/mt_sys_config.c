#include "esp_err.h"
#include "esp_log.h"
#include "stdlib.h"
#include "string.h"

#include "mt_nvs_config.h"
#include "mt_nvs_storage.h"
#include "mt_sys_config.h"

// global define ==============================================================

static const char *TAG = "MT_SYS_CONFIG";

// static func ================================================================

static mt_sys_config_host *mt_sys_config_host_config_new() {
  mt_sys_config_host *host_config = malloc(sizeof(mt_sys_config_host));

  host_config->host = NULL;
  host_config->use_ssl = false;
  host_config->http_port = 0;
  host_config->mqtt_host = NULL;
  host_config->mqtt_port = 0;
  host_config->module_cred_id = NULL;
  host_config->module_cred_key = NULL;

  return host_config;
}

static void mt_sys_config_free_host_config(mt_sys_config_host *host_config) {
  if (host_config == NULL) {
    return;
  }

  if (host_config->host != NULL)
    free(host_config->host);

  if (host_config->mqtt_host != NULL)
    free(host_config->mqtt_host);

  if (host_config->module_cred_id != NULL)
    free(host_config->module_cred_id);

  if (host_config->module_cred_key != NULL)
    free(host_config->module_cred_key);

  free(host_config);
}

// global func ================================================================

esp_err_t mt_sys_config_get_int32_config(char *config_name, int32_t *value) {
  if (config_name == NULL) {
    ESP_LOGE(TAG, "%4d %s config_name NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  // ignore cred key
  if (strcmp(config_name, "mod_key_1") == 0) {
    ESP_LOGE(TAG, "%4d %s unsupport get key", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (mt_nvs_read_int32_config(config_name, value) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_read_int32_config %s failed", __LINE__,
             __func__, config_name);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

char *mt_sys_config_get_string_config(char *config_name) {
  esp_err_t err = ESP_OK;
  char *value_out = NULL;
  size_t value_len = 0;

  if (config_name == NULL) {
    ESP_LOGE(TAG, "%4d %s config_name NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // ignore cred key
  if (strcmp(config_name, "mod_key_1") == 0) {
    ESP_LOGE(TAG, "%4d %s unsupport get key", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  value_out = mt_nvs_read_string_config(config_name, &value_len);
  if (value_out == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_read_string_config %s failed", __LINE__,
             __func__, config_name);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

EXIT:
  if (err != ESP_OK) {
    if (value_out != NULL) {
      free(value_out);
      value_out = NULL;
    }
  }

  return value_out;
}

esp_err_t mt_sys_config_set_int32_config(char *config_name, int32_t value) {
  esp_err_t err = ESP_OK;

  if (config_name == NULL) {
    ESP_LOGE(TAG, "%4d %s config_name NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (mt_nvs_write_int32_config(config_name, value) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_write_int32_config %s failed", __LINE__,
             __func__, config_name);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

EXIT:
  return err;
}

esp_err_t mt_sys_config_set_string_config(char *config_name, char *value) {
  esp_err_t err = ESP_OK;

  if (config_name == NULL) {
    ESP_LOGE(TAG, "%4d %s config_name NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (mt_nvs_write_string_config(config_name, value) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_write_string_config %s failed", __LINE__,
             __func__, config_name);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

EXIT:
  return err;
}

esp_err_t mt_sys_config_set_host_config(mt_sys_config_host *host_config) {
  esp_err_t err = ESP_OK;

  if (host_config == NULL) {
    ESP_LOGE(TAG, "%4d %s host_config NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // host
  if (host_config->host != NULL) {
    err = mt_sys_config_set_string_config("host", host_config->host);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s mt_sys_config_set_string_config host failed",
               __LINE__, __func__);
      err = ESP_ERR_INVALID_RESPONSE;
      goto EXIT;
    }
  }

  // use ssl
  int32_t use_ssl = 0;
  if (host_config->use_ssl == true) {
    use_ssl = 1;
  }
  err = mt_sys_config_set_int32_config("use_ssl", use_ssl);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_sys_config_set_string_config use_ssl failed",
             __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // http port
  if (host_config->http_port <= 0) {
    ESP_LOGE(TAG, "%4d %s invalid http_port %d", __LINE__, __func__,
             host_config->http_port);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }
  err = mt_sys_config_set_int32_config("http_port", host_config->http_port);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_sys_config_set_int32_config http_port failed",
             __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // mqtt host
  if (host_config->mqtt_host != NULL) {
    err = mt_sys_config_set_string_config("mqtt_host", host_config->mqtt_host);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s mt_sys_config_set_string_config mqtt_host failed",
               __LINE__, __func__);
      err = ESP_ERR_INVALID_RESPONSE;
      goto EXIT;
    }
  }

  // mqtt port
  if (host_config->mqtt_port <= 0) {
    ESP_LOGE(TAG, "%4d %s invalid mqtt_port %d", __LINE__, __func__,
             host_config->mqtt_port);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }
  char mqtt_port[12] = "";
  sprintf(mqtt_port, "%d", host_config->mqtt_port);
  err = mt_sys_config_set_string_config("mqtt_port", mqtt_port);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_sys_config_set_string_config mqtt_port failed",
             __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // cred id
  if (host_config->module_cred_id != NULL) {
    err = mt_sys_config_set_string_config("mod_id_1",
                                          host_config->module_cred_id);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s mt_sys_config_set_string_config mod_id_1 failed",
               __LINE__, __func__);
      err = ESP_ERR_INVALID_RESPONSE;
      goto EXIT;
    }
  }

  // cred key
  if (host_config->module_cred_id != NULL) {
    err = mt_sys_config_set_string_config("mod_key_1",
                                          host_config->module_cred_key);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s mt_sys_config_set_string_config mod_key_1 failed",
               __LINE__, __func__);
      err = ESP_ERR_INVALID_RESPONSE;
      goto EXIT;
    }
  }

EXIT:
  return err;
}

mt_sys_config_host *mt_sys_config_get_host_config() {
  esp_err_t err = ESP_OK;
  mt_sys_config_host *host_config = mt_sys_config_host_config_new();

  // host
  host_config->host = mt_sys_config_get_string_config("host");
  if (host_config->host == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_sys_config_get_string_config host NULL", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // use ssl
  int32_t use_ssl = 0;
  err = mt_sys_config_get_int32_config("use_ssl", &use_ssl);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_sys_config_get_int32_config use_ssl failed",
             __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }
  if (use_ssl == 0) {
    host_config->use_ssl = false;
  } else if (use_ssl == 1) {
    host_config->use_ssl = true;
  } else {
    ESP_LOGE(TAG, "%4d %s unexcept use_ssl %d", __LINE__, __func__, use_ssl);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // http port
  err = mt_sys_config_get_int32_config("http_port", &host_config->http_port);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_sys_config_get_int32_config http_port failed",
             __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // mqtt host
  host_config->mqtt_host = mt_sys_config_get_string_config("mqtt_host");
  if (host_config->mqtt_host == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_sys_config_get_string_config mqtt_host NULL",
             __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // mqtt port
  char *mqtt_port = NULL;
  mqtt_port = mt_sys_config_get_string_config("mqtt_port");
  if (mqtt_port == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_sys_config_get_string_config mqtt_port failed",
             __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }
  host_config->mqtt_port = atoi(mqtt_port);
  free(mqtt_port);
  if (host_config->mqtt_port <= 0) {
    ESP_LOGE(TAG, "%4d %s unexcept mqtt_port:%d", __LINE__, __func__,
             host_config->mqtt_port);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // cred id
  host_config->module_cred_id = mt_sys_config_get_string_config("mod_id_1");
  if (host_config->module_cred_id == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_sys_config_get_string_config module_cred_id NULL",
             __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

EXIT:
  if (err != ESP_OK) {
    mt_sys_config_free_host_config(host_config);
    host_config = NULL;
  }

  return host_config;
}
