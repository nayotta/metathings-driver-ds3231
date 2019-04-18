#include "time.h"

#include "esp_log.h"
#include "lwip/apps/sntp.h"
#include "lwip/err.h"
#include "sys/time.h"

#include "mt_nvs_storage.h"
#include "mt_rtc_time.h"
#include "mt_smartconfig.h"
#include "mt_sntp.h"

static const char *TAG = "MT_SNTP";

#define MT_SNTP_NVS_YEAR "SNTP_YEAR"
#define MT_SNTP_NVS_MON "SNTP_MON"
#define MT_SNTP_NVS_DAY "SNTP_DAY"
#define MT_SNTP_SYNC_RTC_TIME (2 * 60)  // 2分钟
#define MT_SNTP_FROM_RTC_TIMEOUT (30)  // 30秒

extern EventGroupHandle_t WIFI_EVENT_GROUP;
bool MT_SNTP_SYNCED = false;

typedef struct {
  int year;
  int mon;
  int day;
} mt_sntp_nvs_t;

// static func ================================================================
static int mt_sntp_read_nvs_config(mt_sntp_nvs_t *last_sync_time) {
  if (mt_nvs_read_int32_config(MT_SNTP_NVS_YEAR, &last_sync_time->year) ==
      false) {
    ESP_LOGE(TAG, "%d mt_nvs_write_int32_config failed", __LINE__);
    return ESP_ERR_INVALID_STATE;
  }

  if (mt_nvs_read_int32_config(MT_SNTP_NVS_MON, &last_sync_time->mon) ==
      false) {
    ESP_LOGE(TAG, "%d mt_nvs_write_int32_config failed", __LINE__);
    return ESP_ERR_INVALID_STATE;
  }

  if (mt_nvs_read_int32_config(MT_SNTP_NVS_DAY, &last_sync_time->day) ==
      false) {
    ESP_LOGE(TAG, "%d mt_nvs_write_int32_config failed", __LINE__);
    return ESP_ERR_INVALID_STATE;
  }

  return ESP_OK;
}

static int mt_sntp_write_nvs_config(struct tm *target_sync_time) {
  if (mt_nvs_write_int32_config(MT_SNTP_NVS_YEAR, target_sync_time->tm_year) ==
      false) {
    ESP_LOGE(TAG, "%d mt_nvs_write_int32_config failed", __LINE__);
    return ESP_ERR_INVALID_STATE;
  }

  if (mt_nvs_write_int32_config(MT_SNTP_NVS_MON, target_sync_time->tm_mon) ==
      false) {
    ESP_LOGE(TAG, "%d mt_nvs_write_int32_config failed", __LINE__);
    return ESP_ERR_INVALID_STATE;
  }

  if (mt_nvs_write_int32_config(MT_SNTP_NVS_DAY, target_sync_time->tm_mday) ==
      false) {
    ESP_LOGE(TAG, "%d mt_nvs_write_int32_config failed", __LINE__);
    return ESP_ERR_INVALID_STATE;
  }

  return ESP_OK;
}

static void mt_sntp_check_nvs_inited() {
  int32_t data = 0;

  if (mt_nvs_read_int32_config(MT_SNTP_NVS_YEAR, &data) == false) {
    if (mt_nvs_write_int32_config(MT_SNTP_NVS_YEAR, 0) == false) {
      ESP_LOGE(TAG, "%d mt_nvs_write_int32_config failed", __LINE__);
      return;
    }
  }

  if (mt_nvs_read_int32_config(MT_SNTP_NVS_MON, &data) == false) {
    if (mt_nvs_write_int32_config(MT_SNTP_NVS_MON, 0) == false) {
      ESP_LOGE(TAG, "%d mt_nvs_write_int32_config failed", __LINE__);
      return;
    }
  }

  if (mt_nvs_read_int32_config(MT_SNTP_NVS_DAY, &data) == false) {
    if (mt_nvs_write_int32_config(MT_SNTP_NVS_DAY, 0) == false) {
      ESP_LOGE(TAG, "%d mt_nvs_write_int32_config failed", __LINE__);
      return;
    }
  }
}

static bool mt_sntp_check_diff_now_and_nvs(mt_sntp_nvs_t nvs, struct tm now) {
  if (nvs.year < 2019 || now.tm_year + 1900 < 2019) {
    return false;
  }

  if (nvs.year != now.tm_year + 1900) {
    return false;
  }

  if (nvs.mon != now.tm_mon) {
    return false;
  }

  if (nvs.day != now.tm_mday) {
    return false;
  }

  return true;
}

static void mt_sntp_from_rtc_loop() {
  struct tm timeinfo = {0};
  struct timeval tv;
  int count = 0;
  time_t now = 0;

  ESP_LOGI(TAG, "mt_sntp_from_rtc_task created");

  setenv("TZ", "CST-8", 1);
  tzset();

  while (count < MT_SNTP_FROM_RTC_TIMEOUT) {
    if (mt_rtc_time_get_time(&timeinfo) == ESP_OK) {
      ESP_LOGI(TAG, "sync time from rtc module");

      // printf("mt_rtc_time_get_time time: %04d-%02d-%02d %02d:%02d:%02d\n",
      //       timeinfo.tm_year, timeinfo.tm_mon + 1, timeinfo.tm_mday,
      //       timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
      timeinfo.tm_year -= 1900;
      tv.tv_sec = mktime(&timeinfo);
      settimeofday(&tv, NULL);

      // time(&now);
      // localtime_r(&now, &timeinfo);
      // printf("mt_rtc_time_get_time sys_time: %04d-%02d-%02d
      // %02d:%02d:%02d\n",
      //       timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
      //       timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

      goto EXIT;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    count++;
  }

  ESP_LOGW(TAG, "sync time from rtc module timeout");

EXIT:
  ESP_LOGW(TAG, "mt_sntp_from_rtc_task deleted");
  vTaskDelete(NULL);
}

static void mt_sntp_init() {
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_init();
  setenv("TZ", "CST-8", 1);
  tzset();
}

static void mt_sntp_loop() {
  time_t now = 0;
  struct tm timeinfo = {0};
  mt_sntp_nvs_t last_sync_time = {0};

  ESP_LOGI(TAG, "mt_sntp_task created");

  // wait 2 second for rtc module sync time first
  vTaskDelay(2 * 1000 / portTICK_PERIOD_MS);

  // WIFI_EVENT_GROUP is from mt_smartconfig, if net connected send BIT2 sign
  xEventGroupWaitBits(WIFI_EVENT_GROUP, BIT2, false, false, portMAX_DELAY);

  // init
  mt_sntp_init();

  // wait sync loop(either sync from wifi or sync from rtc module)
  time(&now);
  localtime_r(&now, &timeinfo);

  while (timeinfo.tm_year < (2019 - 1900)) {
    ESP_LOGI(TAG, "wait time sync...");
    vTaskDelay(2 * 1000 / portTICK_PERIOD_MS);
    time(&now);
    localtime_r(&now, &timeinfo);
  }
  ESP_LOGI(TAG, "time sync finish");
  printf("mt_sntp sync time: %04d-%02d-%02d %02d:%02d:%02d\n",
         timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
         timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  MT_SNTP_SYNCED = true;

  // wait time to sync rtc module
  vTaskDelay(MT_SNTP_SYNC_RTC_TIME * 1000 / portTICK_PERIOD_MS);

  // check nvs config created
  mt_sntp_check_nvs_inited();

  // read nvs config saved
  if (mt_sntp_read_nvs_config(&last_sync_time) != ESP_OK) {
    ESP_LOGE(TAG, "%d mt_sntp_read_nvs_config failed", __LINE__);
    goto EXIT;
  }
  printf("debug:%d nvs %d-%d-%d  now %d-%d-%d\n", __LINE__, last_sync_time.year,
         last_sync_time.mon, last_sync_time.day, timeinfo.tm_year + 1900,
         timeinfo.tm_mon, timeinfo.tm_mday);

  // check if need dayly sync rtc time
  // check last sync year, month and day, if not sync rtc module dayly
  if (mt_sntp_check_diff_now_and_nvs(last_sync_time, timeinfo) == false) {
    ESP_LOGI(TAG, "find rtc time not sync today, sync now!!!!!!!");

    time(&now);
    localtime_r(&now, &timeinfo);
    timeinfo.tm_year += 1900;  // must convert!!!

    if (mt_rtc_time_set_time(&timeinfo) != ESP_OK) {
      ESP_LOGE(TAG, "%d mt_rtc_time_set_time failed", __LINE__);
      goto EXIT;
    }

    if (mt_sntp_write_nvs_config(&timeinfo) != ESP_OK) {
      ESP_LOGE(TAG, "%d mt_sntp_write_nvs_config failed", __LINE__);
      goto EXIT;
    }
  }

EXIT:
  ESP_LOGW(TAG, "mt_sntp_task deleted");
  vTaskDelete(NULL);
}

// public func ================================================================
void mt_sntp_task() {
  xTaskCreate(mt_sntp_loop, "MT_SNTP_TASK", 1024 * 2, NULL, 10, NULL);
  xTaskCreate(mt_sntp_from_rtc_loop, "MT_SNTP_FROM_RTC_TASK", 1024 * 2, NULL,
              10, NULL);
}
