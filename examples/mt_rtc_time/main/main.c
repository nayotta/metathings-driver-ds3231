#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "time.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_log.h"

#include "mt_rtc_time.h"

static const char *TAG = "MT_RTC_TIME_EXAMPLE";

#define SDA_GPIO 21
#define SCL_GPIO 19

void app_main() {
  struct tm time = {.tm_year = 2019,
                    .tm_mon = 3,
                    .tm_mday = 17,
                    .tm_hour = 17,
                    .tm_min = 34,
                    .tm_sec = 1};

  ESP_LOGI(TAG, "MT_RTC_TIME_EXAMPLE");

  mt_rtc_time_task(MT_RTC_MODULE_DS1307, SDA_GPIO, SCL_GPIO);
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  // if (mt_rtc_time_set_time(&time) != ESP_OK) {
  // ESP_LOGE(TAG, "%d mt_rtc_time_set_time failed", __LINE__);
  // return;
  //}

  while (1) {
    if (mt_rtc_time_get_time(&time) != ESP_OK) {
      ESP_LOGE(TAG, "%d mt_rtc_time_get_time failed", __LINE__);
      return;
    }

    printf("mt_rtc_time_init rtc_time is %04d-%02d-%02d %02d:%02d:%02d\n",
           time.tm_year, time.tm_mon + 1, time.tm_mday, time.tm_hour,
           time.tm_min, time.tm_sec);

    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }

  ESP_LOGI(TAG, "MT_RTC_TIME_EXAMPLE test over");
}
