#ifndef MT_UITILS_H
#define MT_UITILS_H

#include "stdint.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

#define MQTT_HMAC_TIME_STAMP "1546300800000000000"
#define MQTT_HMAC_NONCE "1024"

int mt_hmac_sha256(const uint8_t *key, int key_size, const uint8_t *id,
                   int id_size, uint8_t *time_stamp, int time_stamp_size,
                   uint8_t *nonce, int nonce_size, uint8_t *hmac);

unsigned char *mt_hmac_sha256_mqtt(const uint8_t *key, int key_size, const uint8_t *id,
                   int id_size);

#endif