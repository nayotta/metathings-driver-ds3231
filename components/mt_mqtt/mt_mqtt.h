#ifndef MT_MQTT_H
#define MT_MQTT_H

#include "stdint.h"
#include "stdio.h"

#include "esp_err.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

#define URI_MAX_SIZE 128
#define TOPIC_MAX_SIZE 512

esp_err_t mt_mqtt_pub_msg(char *topic, uint8_t *buf, int size);

esp_err_t mt_mqtt_init(int mod_index, char *module_id, uint64_t session_id,
                       char *device_id,
                       void (*handle)(char *topic, void *buf, int size));

#endif
