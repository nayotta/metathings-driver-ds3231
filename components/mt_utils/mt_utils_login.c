#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/time.h"

#include "esp_log.h"
#include "esp_system.h"

#include "mt_utils_login.h"

// global define ==============================================================
static const char *TAG = "MT_UTILS_LOGIN";

#define MT_UTILS_LOGIN_MAX_TIME_STR_SIZE 128
#define MT_UTILS_LOGIN_MAX_NONCE_STR_SIZE 32

// global func ================================================================
uint8_t *mt_utils_login_get_time_rfc3339nano_string(uint8_t *time_str_size)
{
    time_t now = 0;
    struct tm *timeinfo = NULL;
    uint8_t *time_str = NULL;
    char temp_str[MT_UTILS_LOGIN_MAX_TIME_STR_SIZE] = "";

    time(&now);
    localtime_r(&now, timeinfo);
    if (timeinfo == NULL)
    {
        ESP_LOGE(TAG, "%3d %s timeinfo null", __LINE__, __func__);
        goto EXIT;
    }

    sprintf(temp_str, "%d-%d-%dT%02d:%02d:%02d.000000000+08:00",
            timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    *time_str_size = strlen(temp_str);
    if (*time_str_size > MT_UTILS_LOGIN_MAX_TIME_STR_SIZE)
    {
        ESP_LOGE(TAG, "%3d %s time_str_size too long", __LINE__, __func__);
        goto EXIT;
    }
    time_str = malloc(*time_str_size);
    memcpy(time_str, temp_str, *time_str_size);

EXIT:
    return time_str;
}

uint32_t mt_utils_login_get_nonce()
{
    return esp_random();
}

uint64_t mt_utils_login_get_random_session()
{
    uint32_t high = esp_random();
    uint32_t low = esp_random();

    return high << 32 + low;
}
