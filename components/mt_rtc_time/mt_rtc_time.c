#include "stdio.h"

#include "esp_err.h"
#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ds1307.h"
#include "ds3231.h"
#include "i2cdev.h"
#include "mt_nvs_storage.h"
#include "mt_rtc_time.h"

static const char *TAG = "MT_RTC_TIME";

#define MT_RTC_TIME_NVS "RTC_TIME_SYNCED"

static enum MT_RTC_MODULE_TYPE RTC_MODULE = MT_RTC_MODULE_DS3231;
static i2c_dev_t RTC_I2C_DEV;
static int SDA_GPIO = 21;
static int SCL_GPIO = 19;
static bool TASK_SYNCED = false;

// static func ================================================================
static esp_err_t mt_rtc_time_init() {
  struct tm time = {.tm_year = 2000,
                    .tm_mon = 1,
                    .tm_mday = 1,
                    .tm_hour = 1,
                    .tm_min = 1,
                    .tm_sec = 1};
  int32_t time_synced = 0;

  while (i2cdev_init() != ESP_OK) {
    ESP_LOGE(TAG, "%d i2cdev_init failed", __LINE__);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }

  switch (RTC_MODULE) {
  case MT_RTC_MODULE_DS1307:
    while (ds1307_init_desc(&RTC_I2C_DEV, 0, SDA_GPIO, SCL_GPIO)) {
      ESP_LOGE(TAG, "%d ds1307_init_desc failed", __LINE__);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    if (ds1307_get_time(&RTC_I2C_DEV, &time) != ESP_OK) {
      ESP_LOGE(TAG, "%d ds1307_get_time failed", __LINE__);
      return ESP_ERR_INVALID_STATE;
    }
    break;

  case MT_RTC_MODULE_DS3231:
    while (ds3231_init_desc(&RTC_I2C_DEV, 0, SDA_GPIO, SCL_GPIO)) {
      ESP_LOGE(TAG, "%d ds3231_init_desc failed", __LINE__);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    if (ds3231_get_time(&RTC_I2C_DEV, &time) != ESP_OK) {
      ESP_LOGE(TAG, "%d ds3231_get_time failed", __LINE__);
      return ESP_ERR_INVALID_STATE;
    }
    break;

  default:
    ESP_LOGE(TAG, "%d error rtc_module type", __LINE__);
    return ESP_ERR_INVALID_STATE;
  }

  // check sync data
  if (time.tm_year < 2019) {
    ESP_LOGW(TAG, "%4d %s sync time:%d error, reset sync state", __LINE__,
             __func__, time.tm_year);
    if (mt_nvs_write_int32_config(MT_RTC_TIME_NVS, 0) == false) {
      ESP_LOGE(TAG, "%4d %s mt_nvs_write_int32_config MT_RTC_TIME_NVS failed",
               __LINE__, __func__);
    }
  }

  // check nvs storage
  if (mt_nvs_read_int32_config(MT_RTC_TIME_NVS, &time_synced) == false) {
    ESP_LOGI(TAG, "%d time not synced", __LINE__);
    if (mt_nvs_write_int32_config(MT_RTC_TIME_NVS, 0) == false) {
      ESP_LOGE(TAG, "%d mt_nvs_write_int32_config failed", __LINE__);
      return ESP_ERR_INVALID_STATE;
    }
  }

  printf("mt_rtc_time_init rtc_time is %04d-%02d-%02d %02d:%02d:%02d, "
         "is_synced:%d\n",
         time.tm_year, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min,
         time.tm_sec, time_synced);

  TASK_SYNCED = true;

  return ESP_OK;
}

static void mt_rtc_time_task_loop() {
  int32_t time_synced = 0;

  if (mt_nvs_read_int32_config(MT_RTC_TIME_NVS, &time_synced) == false) {
    ESP_LOGW(TAG, "%4d %s mt_nvs_read_int32_config MT_RTC_TIME_NVS failed",
             __LINE__, __func__);
    if (mt_nvs_write_int32_config(MT_RTC_TIME_NVS, 0) == false) {
      ESP_LOGE(TAG, "%4d %s mt_nvs_write_int32_config MT_RTC_TIME_NVS failed",
               __LINE__, __func__);
    }
  }

  if (mt_rtc_time_init() != ESP_OK) {
    ESP_LOGE(TAG, "%d mt_rtc_time_setup failed", __LINE__);
    vTaskDelete(NULL);
  }

  vTaskDelete(NULL);
}

// public func ================================================================
esp_err_t mt_rtc_time_get_time(struct tm *time) {
  int32_t time_synced = 0;

  // check if task synced
  if (TASK_SYNCED != true) {
    ESP_LOGE(TAG, "%d TASK_SYNCED not finish", __LINE__);
    return ESP_ERR_INVALID_STATE;
  }

  // check if time synced
  if (mt_nvs_read_int32_config(MT_RTC_TIME_NVS, &time_synced) == false) {
    ESP_LOGE(TAG, "%d mt_nvs_read_int32_config failed", __LINE__);
    return ESP_ERR_INVALID_STATE;
  }

  if (time_synced != 1) {
    ESP_LOGE(TAG, "%d time not synced", __LINE__);
    return ESP_ERR_INVALID_STATE;
  }

  switch (RTC_MODULE) {
  case MT_RTC_MODULE_DS1307:
    if (ds1307_get_time(&RTC_I2C_DEV, time) != ESP_OK) {
      ESP_LOGE(TAG, "%d ds1307_get_time failed", __LINE__);
    }
    break;

  case MT_RTC_MODULE_DS3231:
    if (ds3231_get_time(&RTC_I2C_DEV, time) != ESP_OK) {
      ESP_LOGE(TAG, "%d ds3231_get_time failed", __LINE__);
    }
    break;

  default:
    ESP_LOGE(TAG, "%d error rtc_module type", __LINE__);
    return ESP_ERR_INVALID_STATE;
  }

  if (time->tm_year < 2019) {
    ESP_LOGE(TAG, "%4d %s sync year error", __LINE__, __func__);
    if (mt_nvs_write_int32_config(MT_RTC_TIME_NVS, 0) == false) {
      ESP_LOGE(TAG, "%4d %s mt_nvs_write_int32_config failed", __LINE__,
               __func__);
      return ESP_ERR_INVALID_STATE;
    }
    return ESP_ERR_INVALID_STATE;
  }

  return ESP_OK;
}

esp_err_t mt_rtc_time_set_time(struct tm *time) {
  // check if task synced
  if (TASK_SYNCED != true) {
    ESP_LOGE(TAG, "%d TASK_SYNCED not finish", __LINE__);
    return ESP_ERR_INVALID_STATE;
  }

  // simple check time
  if (time->tm_year < 2019) {
    ESP_LOGE(TAG, "%d mt_rtc_time_set_time error year:%d", __LINE__,
             time->tm_year);
    return ESP_ERR_INVALID_STATE;
  }

  switch (RTC_MODULE) {
  case MT_RTC_MODULE_DS1307:
    if (ds1307_set_time(&RTC_I2C_DEV, time) != ESP_OK) {
      ESP_LOGE(TAG, "%d ds1307_set_time failed", __LINE__);
    }
    break;

  case MT_RTC_MODULE_DS3231:
    if (ds3231_set_time(&RTC_I2C_DEV, time) != ESP_OK) {
      ESP_LOGE(TAG, "%d ds3231_set_time failed", __LINE__);
    }
    break;

  default:
    ESP_LOGE(TAG, "%d error rtc_module type", __LINE__);
    return ESP_ERR_INVALID_STATE;
  }

  if (mt_nvs_write_int32_config(MT_RTC_TIME_NVS, 1) == false) {
    ESP_LOGE(TAG, "%d mt_nvs_write_int32_config", __LINE__);
    return ESP_ERR_INVALID_STATE;
  };

  return ESP_OK;
}

esp_err_t mt_rtc_time_syncd(bool *sync) { return ESP_OK; }

void mt_rtc_time_task(enum MT_RTC_MODULE_TYPE rtc_module, int sda_gpio,
                      int scl_gpio) {
  RTC_MODULE = rtc_module;
  SDA_GPIO = sda_gpio;
  SCL_GPIO = scl_gpio;

  xTaskCreate(mt_rtc_time_task_loop, "MT_RTC_TIME_TASK", 1024 * 2, NULL, 10,
              NULL);
}
