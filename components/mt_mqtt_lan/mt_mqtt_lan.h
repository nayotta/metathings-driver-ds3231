#ifndef MT_MQTT_LAN_H
#define MT_MQTT_LAN_H

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

int mqtt_pub_msg(char *topic, uint8_t *buf, int size);

int mqtt_init(char *host, char *port, char *username, char *password,
              char *device_id, void (*handle)(void *buf, int size));

#endif
