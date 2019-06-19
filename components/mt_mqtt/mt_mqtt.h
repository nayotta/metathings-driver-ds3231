#ifndef MT_MQTT_H
#define MT_MQTT_H

#include "stdint.h"
#include "stdio.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

#define URI_MAX_SIZE 128
#define TOPIC_MAX_SIZE 128

int mt_mqtt_pub_msg(char *topic, uint8_t *buf, int size);

int mt_mqtt_init(char *host, char *port, char *username, char *password,
                 char *device_id, void (*handle)(void *buf, int size));

void mt_mqtt_task();

#endif
