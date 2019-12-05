#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "time.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "mt_rtc_time.h"
#include "mt_smartconfig.h"
#include "mt_sntp.h"
#include "mt_nvs_storage.h"

// global config ==============================================================
static const char *TAG = "MT_RTC_TIME_EXAMPLE";

int SDA_GPIO = 32;
int SCL_GPIO = 33;

int LIGHT_PIN = 14;
int LIGHT_PIN_ON_LEVEL = 0;

int BTN_PIN = 35;
int BTN_PIN_ON_LEVEL = 0;

// global func ================================================================
void app_main()
{
  // nvs
  mt_nvs_init();

  // smartconfig
  mt_smartconfig_task(LIGHT_PIN, LIGHT_PIN_ON_LEVEL, BTN_PIN, BTN_PIN_ON_LEVEL);

  // sntp task
  mt_sntp_task();

  struct tm time = {.tm_year = 1970};

  ESP_LOGI(TAG, "MT_RTC_TIME_EXAMPLE");

  mt_rtc_time_task(MT_RTC_MODULE_DS1307, SDA_GPIO, SCL_GPIO);
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  while (1)
  {
    if (mt_rtc_time_get_time(&time) != ESP_OK)
    {
      ESP_LOGE(TAG, "%d mt_rtc_time_get_time failed", __LINE__);
      goto EXIT;
    }

    printf("get time is %04d-%02d-%02d %02d:%02d:%02d\n",
           time.tm_year, time.tm_mon + 1, time.tm_mday, time.tm_hour,
           time.tm_min, time.tm_sec);

  EXIT:
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }

  ESP_LOGI(TAG, "MT_RTC_TIME_EXAMPLE test over");
}
