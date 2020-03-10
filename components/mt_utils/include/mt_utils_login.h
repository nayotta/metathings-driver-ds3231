#ifndef _MT_UTILS_LOGIN_H
#define _MT_UTILS_LOGIN_H

#include "esp_err.h"
#include "mt_module_http.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

time_t mt_utils_login_get_time_now();

char *mt_utils_login_time_to_ms_string(time_t time_in);

uint8_t *mt_utils_login_get_time_rfc3339nano_string(time_t time_in,
                                                    uint8_t *time_str_size);

uint32_t mt_utils_login_get_nonce();

char *mt_utils_login_get_issue_token_data(mt_module_http_t *module_http);

char *mt_utils_login_get_heartbeat_data(mt_module_http_t *module_http);

#endif
