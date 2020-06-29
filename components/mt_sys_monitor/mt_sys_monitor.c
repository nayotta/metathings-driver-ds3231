
#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "mt_nvs_storage.h"
#include "mt_sys_monitor.h"

// global define ==============================================================

static const char *TAG = "MT_SYS_MONITOR";
static int32_t ERROR_COUNT = 0;

// static func ================================================================

static mt_sys_monitor_state *mt_sys_monitor_state_new() {
  mt_sys_monitor_state *state = malloc(sizeof(mt_sys_monitor_state));

  state->startup = 0;
  state->restart_count = 0;
  state->free = 0;
  state->error_count = 0;

  return state;
}

static esp_err_t
mt_sys_monitor_read_restart_count_from_nvs(int32_t *restart_count_out) {
  if (mt_nvs_read_int32_config("sys_m_restart", restart_count_out) == false) {
    ESP_LOGW(TAG, "%4d %s sys_m_restart record not exist,create now", __LINE__,
             __func__);
    if (mt_nvs_write_int32_config("sys_m_restart", 0) == false) {
      ESP_LOGE(TAG, "%4d %s sys_m_restart create failed", __LINE__, __func__);
      return ESP_ERR_INVALID_RESPONSE;
    }
  }

  return ESP_OK;
}

static void restart_loop() {
  while (1) {
    vTaskDelay(5000 / portTICK_RATE_MS);
    esp_restart();
  }
}

// global func ================================================================

esp_err_t mt_sys_monitor_set_restart() {
  xTaskCreate((TaskFunction_t)restart_loop, "restart_task", 2048, NULL, 8,
              NULL);

  return ESP_OK;
}

esp_err_t mt_sys_monitor_set_restart_count() {
  esp_err_t err = ESP_OK;
  int32_t restart_count = 0;

  err = mt_sys_monitor_read_restart_count_from_nvs(&restart_count);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_sys_monitor_read_restart_count_from_nvs failed",
             __LINE__, __func__);
    return err;
  }

  restart_count++;
  if (mt_nvs_write_int32_config("sys_m_restart", restart_count) == false) {
    ESP_LOGE(TAG, "%4d %s sys_m_restart set failed", __LINE__, __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

esp_err_t mt_sys_monitor_set_error_count() {
  ERROR_COUNT++;

  return ESP_OK;
}

esp_err_t mt_sys_monitor_get_startup(int32_t *startup_out) {
  *startup_out = esp_timer_get_time() / 1000000;

  return ESP_OK;
}

esp_err_t mt_sys_monitor_get_restart_count(int32_t *restart_count_out) {
  esp_err_t err = ESP_OK;

  err = mt_sys_monitor_read_restart_count_from_nvs(restart_count_out);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_sys_monitor_read_restart_count_from_nvs failed",
             __LINE__, __func__);
    return err;
  }

  return ESP_OK;
}

esp_err_t mt_sys_monitor_get_free(float *free_out) {
  uint32_t free = 0;
  free = esp_get_free_heap_size();
  *free_out = free / 1.0;

  return ESP_OK;
}

esp_err_t mt_sys_monitor_get_error_count(int32_t *error_count_out) {
  *error_count_out = ERROR_COUNT;

  return ESP_OK;
}

mt_sys_monitor_state *mt_sys_monitor_get_state() {
  esp_err_t err = ESP_OK;
  mt_sys_monitor_state *state = mt_sys_monitor_state_new();

  // get startup
  err = mt_sys_monitor_get_startup(&state->startup);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_sys_monitor_get_startup failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // get restart count
  err = mt_sys_monitor_get_restart_count(&state->restart_count);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_sys_monitor_get_restart_count failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // get free
  err = mt_sys_monitor_get_free(&state->free);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_sys_monitor_get_free failed", __LINE__, __func__);
    goto EXIT;
  }

  // get error count
  err = mt_sys_monitor_get_error_count(&state->error_count);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_sys_monitor_get_error_count failed", __LINE__,
             __func__);
    goto EXIT;
  }

EXIT:
  if (err != ESP_OK) {
    free(state);
    state = NULL;
  }

  return state;
}
