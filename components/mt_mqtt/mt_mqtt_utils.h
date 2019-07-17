#ifndef _MT_MQTT_UTILS_H
#define _MT_MQTT_UTILS_H

#include "esp_err.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

esp_err_t mt_mqtt_utils_get_session_id_from_topic(char *topic, uint64_t *session_id);

#endif
