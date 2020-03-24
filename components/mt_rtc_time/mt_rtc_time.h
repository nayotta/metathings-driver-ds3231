#ifndef _MT_RTC_TIME_H
#define _MT_RTC_TIME_H

#include "esp_err.h"
#include "stdio.h"
#include "time.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

enum MT_RTC_MODULE_TYPE { MT_RTC_MODULE_DS1307 = 0, MT_RTC_MODULE_DS3231 };

esp_err_t mt_rtc_time_set_time(struct tm *time);

esp_err_t mt_rtc_time_get_time(struct tm *time);

esp_err_t mt_rtc_time_syncd(bool *sync);

void mt_rtc_time_task(enum MT_RTC_MODULE_TYPE rtc_module, int sda_gpio,
                      int scl_gpio);

#endif
