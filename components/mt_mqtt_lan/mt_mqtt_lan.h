#ifndef MT_MQTT_LAN_H
#define MT_MQTT_LAN_H

#include "esp_err.h"
#include "mqtt_client.h"

// define =====================================================================

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

// func =======================================================================

esp_err_t mqtt_pub_msg(char *topic, uint8_t *buf, int size);

esp_err_t mqtt_init(int index_in, char *module_id, uint64_t session_id,
                    char *device_id,
                    void (*handle)(char *topic, void *buf, int size));

esp_err_t mt_mqtt_lan_update_session_id(uint64_t session_id);

#endif
