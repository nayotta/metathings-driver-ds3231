#include "rs232_charge001_sent_manage.h"

// static define ==============================================================

static const char *TAG = "RS232_CHARGE001_SENT_MANAGE";

static SemaphoreHandle_t SENT_LOCK = NULL;
static long SENT_TIMEOUT = 3000;
static rs232_dev_config_t *DEV_CONFIG = NULL;

// static func ================================================================

static esp_err_t sent_lock_take(long timeout) {
  if (SENT_LOCK == NULL) {
    SENT_LOCK = xSemaphoreCreateMutex();
  }
  if (xSemaphoreTake(SENT_LOCK, (portTickType)timeout) == pdTRUE) {
    return ESP_OK;
  }
  return ESP_ERR_INVALID_RESPONSE;
}

static void sent_lock_release() {
  if (SENT_LOCK == NULL) {
    SENT_LOCK = xSemaphoreCreateMutex();
  }

  xSemaphoreGive(SENT_LOCK);
}

static esp_err_t sent_cmd(uint8_t *buf, int32_t size) {
  esp_err_t err = ESP_OK;

  if (DEV_CONFIG == NULL) {
    ESP_LOGE(TAG, "%4d %s DEV_CONFIG NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_STATE;
  }

  err = rs232_dev_write(DEV_CONFIG, buf, size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_dev_write failed", __LINE__, __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

// global func ================================================================

esp_err_t rs232_charge001_sent_manage_init(rs232_dev_config_t *dev_config) {
  if (dev_config == NULL) {
    ESP_LOGE(TAG, "%4d %s dev_config NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  } else {
    if (dev_config->uart_config == NULL) {
      ESP_LOGE(TAG, "%4d %s dev_config->uart_config NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }
  }

  DEV_CONFIG = dev_config;
  ESP_LOGI(TAG, "%4d %s init success", __LINE__, __func__);

  return ESP_OK;
}

esp_err_t rs232_charge001_sent_manage_sent_and_wait_finish(
    uint8_t *buf, int buf_size, int timeout, esp_err_t (*msg_handle)()) {
  esp_err_t err = ESP_OK;
  int interval = 20;
  int count = timeout / interval + 1;

  // ESP_LOGI(TAG, "%4d %s sent cmd:%s", __LINE__, __func__, buf);

  if (sent_lock_take(SENT_TIMEOUT) != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s sent_lock_take timeout", __LINE__, __func__);
    return ESP_ERR_TIMEOUT;
  }

  // sent cmd
  err = sent_cmd(buf, buf_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s sent_cmd failed", __LINE__, __func__);
    goto EXIT;
  }

  // wait finish
  while (count--) {
    // wait msg finish
    if (msg_handle != NULL) {
      err = msg_handle();
      if (err == ESP_OK) {
        goto EXIT;
      }
    }
    vTaskDelay(interval / portTICK_PERIOD_MS);
  }

  ESP_LOGW(TAG, "%4d %s cmd timeout", __LINE__, __func__);
  err = ESP_ERR_TIMEOUT;

EXIT:
  sent_lock_release();

  return err;
}
