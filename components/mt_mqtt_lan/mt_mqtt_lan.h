#ifndef MT_MQTT_LAN_H
#define MT_MQTT_LAN_H

#include "mqtt_client.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

int mqtt_pub_msg(char *topic, uint8_t *buf, int size);

int mqtt_init(int index_in,
              char *module_id, uint64_t session_id,char *device_id,
              void (*handle)(char *topic, void *buf, int size));

#endif
