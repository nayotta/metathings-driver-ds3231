#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/time.h"
#include "time.h"

#include "esp_log.h"
#include "esp_system.h"

#include "mt_utils_login.h"

// global define ==============================================================
static const char *TAG = "MT_UTILS_LOGIN";

#define MT_UTILS_LOGIN_MAX_TIME_STR_SIZE 128
#define MT_UTILS_LOGIN_MAX_NONCE_STR_SIZE 32

// global func ================================================================
time_t mt_utils_login_get_time_now()
{
  time_t now = 0;
  time(&now);

  return now;
}

char *mt_utils_login_time_to_ms_string(time_t time_in)
{
  char temp_str[MT_UTILS_LOGIN_MAX_TIME_STR_SIZE] = "";
  char *time_str = NULL;

  sprintf(temp_str, "%ld000000000", time_in);
  time_str = malloc(strlen(temp_str) + 1);
  memcpy(time_str, temp_str, strlen(temp_str));
  time_str[strlen(temp_str)] = '\0';

  return time_str;
}

uint8_t *mt_utils_login_get_time_rfc3339nano_string(time_t time_in, uint8_t *time_str_size)
{
  struct tm timeinfo = {0};
  uint8_t *time_str = NULL;
  char temp_str[MT_UTILS_LOGIN_MAX_TIME_STR_SIZE] = "";

  localtime_r(&time_in, &timeinfo);

  sprintf(temp_str, "%d-%02d-%02dT%02d:%02d:%02d.000000000+00:00",
          timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
          timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  *time_str_size = strlen(temp_str);
  if (*time_str_size > MT_UTILS_LOGIN_MAX_TIME_STR_SIZE)
  {
    ESP_LOGE(TAG, "%3d %s time_str_size too long", __LINE__, __func__);
    goto EXIT;
  }
  time_str = malloc(*time_str_size + 1);
  memcpy(time_str, temp_str, *time_str_size);
  time_str[*time_str_size] = '\0';

EXIT:
  return time_str;
}

uint32_t mt_utils_login_get_nonce() { return esp_random(); }