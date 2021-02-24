#include "rs232_sim_air720h_sent_manage.h"

#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#include "freertos/semphr.h"

#include "rs232_dev.h"

#include "rs232_sim_air720h_recv_manage.h"

// static define ==============================================================

static const char *TAG = "rs232_sim_air720_sent_manage";

static SemaphoreHandle_t SENT_LOCK = NULL;
static long SENT_TIMEOUT = 5000;
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

static esp_err_t sent_cmd(const char *buf) {
  int sent_size = 0;
  int sent_ret_size = 0;

  if (DEV_CONFIG == NULL) {
    ESP_LOGE(TAG, "%4d %s DEV_CONFIG NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_STATE;
  }

  sent_size = strlen(buf);

  sent_ret_size = uart_write_bytes(DEV_CONFIG->uart_num, buf, sent_size);
  ESP_LOGW(TAG, "%4d %s uart_port:%d point:%p", __LINE__, __func__,
           DEV_CONFIG->uart_num, DEV_CONFIG);
  if (sent_ret_size != sent_size) {
    ESP_LOGE(TAG, "%4d %s sent:%d but sent_ret:%d uart_port:%d point:%p",
             __LINE__, __func__, sent_size, sent_ret_size, DEV_CONFIG->uart_num,
             DEV_CONFIG);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

// global func ================================================================

esp_err_t rs232_sim_air720h_sent_manage_init(rs232_dev_config_t *dev_config) {
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

esp_err_t rs232_sim_air720h_sent_manage_sent(uint8_t *buf, int buf_size) {
  esp_err_t err = ESP_OK;

  if (sent_lock_take(SENT_TIMEOUT) != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s sent_lock_take timeout", __LINE__, __func__);
    return ESP_ERR_TIMEOUT;
  }

  err = sent_cmd((const char *)buf);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s sent_cmd failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  sent_lock_release();

  return err;
}

esp_err_t rs232_sim_air720h_sent_manage_sent_and_wait_finish(
    uint8_t *buf, int buf_size, int timeout, esp_err_t (*msg_handle)(),
    esp_err_t (*ack_handle)()) {
  esp_err_t err = ESP_OK;
  int interval = 20;
  int count = timeout / interval + 1;
  bool msg_finish = false;
  bool ack_finish = false;

  // reset ack cache
  rs232_sim_air720h_recv_manage_reset_ack();

  ESP_LOGI(TAG, "%4d %s sent cmd:%s", __LINE__, __func__, buf);

  // check arg
  if (msg_handle == NULL && ack_handle == NULL) {
    // ESP_LOGW(TAG, "%4d %s msg_handle and ack_handle both NULL", __LINE__,
    //        __func__);
  }

  if (msg_handle == NULL) {
    msg_finish = true;
  }

  if (ack_handle == NULL) {
    ack_finish = true;
  }

  if (sent_lock_take(SENT_TIMEOUT) != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s sent_lock_take timeout", __LINE__, __func__);
    return ESP_ERR_TIMEOUT;
  }

  // sent cmd
  err = sent_cmd((const char *)buf);
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
        msg_finish = true;
      }
    }

    // wait ack finish
    if (ack_handle != NULL) {
      err = ack_handle();
      if (err == ESP_OK) {
        ack_finish = true;
      }
    }

    // check all finish
    if (msg_finish == true && ack_finish == true) {
      // ESP_LOGI(TAG, "%4d %s cmd wait and finished", __LINE__, __func__);
      goto EXIT;
    }

    vTaskDelay(interval / portTICK_PERIOD_MS);
  }

  ESP_LOGW(TAG, "%4d %s cmd timeout", __LINE__, __func__);
  err = ESP_ERR_TIMEOUT;

EXIT:
  sent_lock_release();

  return err;
}
