#ifndef MT_HTTP_CLIENT
#define MT_HTTP_CLIENT

#include "esp_err.h"
#include "esp_http_client.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

esp_err_t mt_http_client_post_request(esp_http_client_handle_t client,
                                      char *token, char *post_data);

#endif
