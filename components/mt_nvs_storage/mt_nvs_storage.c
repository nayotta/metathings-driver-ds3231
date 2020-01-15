#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "stdio.h"

#include "mt_nvs_storage.h"

int IS_INITED = 0;
nvs_handle config_handle = 0;
static const char *TAG = "MT_NVS_STOREGE";

// nvs flash init and set a handle
bool mt_nvs_init() {
  esp_err_t err;

  // Initialize NVS
  ESP_LOGI(TAG, "nvs init");
  err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  IS_INITED = 1;

  err = nvs_open("storage", NVS_READWRITE, &config_handle);
  if (err != ESP_OK) {
    ESP_LOGI(TAG, "Error () opening NVS handle!");
    return false;
  }
  ESP_LOGI(TAG, "nvs init ok");

  return true;
}

// get nvs handle
nvs_handle mt_nvs_get_handle() {
  if (config_handle) {
    return config_handle;
  } else {
    if (mt_nvs_init() == true) {
      return config_handle;
    } else {
      return 0;
    }
  }
}

// check nvs flash init
static void mt_nvs_check_init() {
  if (IS_INITED == 0) {
    mt_nvs_init();
  } else {
    return;
  }
}

uint8_t *mt_nvs_read_byte_config(char *config_name, int *len) {
  mt_nvs_check_init();
  esp_err_t err;
  uint8_t *buf = NULL;

  // get byte size first
  err = nvs_get_blob(config_handle, config_name, NULL, (size_t *)len);
  switch (err) {
    case ESP_OK:
      if (*len <= 1) {
        ESP_LOGE(TAG, "%s read size %s NULL", __func__, config_name);
        return NULL;
      }
      break;
    case ESP_ERR_NVS_NOT_FOUND:
      ESP_LOGE(TAG, "%s read size %s not initialized!", __func__, config_name);
      return NULL;
      break;
    default:
      ESP_LOGE(TAG, "%s read size %s error", __func__, config_name);
      return NULL;
  }

  // get byte
  buf = malloc(*len);  // free in func
  err = nvs_get_blob(config_handle, config_name, buf, (size_t *)len);
  switch (err) {
    case ESP_OK:
      if (*len <= 1) {
        ESP_LOGE(TAG, "%s read %s NULL", __func__, config_name);
        goto EXIT;
      } else {
        // ESP_LOGI(TAG, "%s read %s,len=%d", __func__, config_name, (int)*len);
        return buf;
      }
      break;
    case ESP_ERR_NVS_NOT_FOUND:
      ESP_LOGE(TAG, "%s read %s not initialized!", __func__, config_name);
      goto EXIT;
      break;
    default:
      ESP_LOGE(TAG, "%s read %s error", __func__, config_name);
      goto EXIT;
  }
  return buf;

EXIT:
  free(buf);
  return NULL;
}

char *mt_nvs_read_string_config(char *config_name, size_t *len) {
  mt_nvs_check_init();
  esp_err_t err;
  char *ret_buf = NULL;

  err = nvs_get_str(config_handle, config_name, NULL, len);
  switch (err) {
    case ESP_OK:
      if (*len > 0) {
        ret_buf = malloc(*len);
        // ESP_LOGI(TAG, "mt_nvs_read_string_config len=%d", *len);
      } else {
        return NULL;
      }
      break;
    case ESP_ERR_NVS_NOT_FOUND:
      ESP_LOGI(TAG, "READ config [%s] not initialized!", config_name);
      return NULL;
      break;
    default:
      ESP_LOGI(TAG, "READ config error () reading!\n");
      return NULL;
  }

  err = nvs_get_str(config_handle, config_name, ret_buf, len);
  switch (err) {
    case ESP_OK:
      // ESP_LOGI(TAG, "READ config [%s]=[%s],len=%d", config_name,
      // config_value,
      //         (int)len);
      return ret_buf;
      break;
    case ESP_ERR_NVS_NOT_FOUND:
      ESP_LOGI(TAG, "READ config [%s] not initialized!", config_name);
      return NULL;
      break;
    default:
      ESP_LOGI(TAG, "READ config error () reading!\n");
      return NULL;
  }
  return NULL;
}

bool mt_nvs_read_int32_config(char *config_name, int32_t *config_value) {
  mt_nvs_check_init();
  esp_err_t err;

  err = nvs_get_i32(config_handle, config_name, config_value);
  switch (err) {
    case ESP_OK:
      // ESP_LOGI(TAG, "READ config [%s]=[%d]", config_name,
      // (int)*config_value);
      return true;
      break;
    case ESP_ERR_NVS_NOT_FOUND:
      ESP_LOGI(TAG, "READ config [%s] not initialized!", config_name);
      return false;
      break;
    default:
      ESP_LOGE(TAG, "READ config error () reading!\n");
      return false;
  }
  return true;
}

bool mt_nvs_write_string_config(char *config_name, char *config_value) {
  mt_nvs_check_init();
  esp_err_t err;

  // ESP_LOGI(TAG, "write begin");
  err = nvs_set_str(config_handle, config_name, config_value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "WRITE config [%s] error", config_name);
    return false;
  } else {
    // ESP_LOGI(TAG, "WRITE config [%s] ok", config_name);
  }
  err = nvs_commit(config_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "COMMIT config [%s] error", config_name);
    return false;
  } else {
    // ESP_LOGI(TAG, "COMMIT config [%s] ok", config_name);
  }

  return true;
}

bool mt_nvs_write_int32_config(char *config_name, int32_t config_value) {
  mt_nvs_check_init();
  esp_err_t err;

  err = nvs_set_i32(config_handle, config_name, config_value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "WRITE config [%s] error", config_name);
    return false;
  } else {
    // ESP_LOGI(TAG, "WRITE config [%s] ok", config_name);
  }
  err = nvs_commit(config_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "COMMIT config [%s] error", config_name);
    return false;
  } else {
    // ESP_LOGI(TAG, "COMMIT config [%s] ok", config_name);
  }

  return true;
}

bool mt_nvs_write_byte_config(char *config_name, int len, uint8_t *buf) {
  mt_nvs_check_init();
  esp_err_t err;

  err = nvs_set_blob(config_handle, config_name, buf, len);
  switch (err) {
    case ESP_OK:
      if (len <= 1) {
        ESP_LOGE(TAG, "%s read %s NULL", __func__, config_name);
        return false;
      } else {
        // ESP_LOGI(TAG, "%s read %s,len=%d", __func__, config_name, (int)len);
        return true;
      }
      break;
    case ESP_ERR_NVS_NOT_FOUND:
      ESP_LOGE(TAG, "%s read %s not initialized!", __func__, config_name);
      return false;
      break;
    default:
      ESP_LOGE(TAG, "%s read %s error", __func__, config_name);
      return false;
  }

  return true;
}
