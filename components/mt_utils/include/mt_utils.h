#ifndef MT_UITILS_H
#define MT_UITILS_H

#include "esp_err.h"
#include "stdint.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

#define MQTT_HMAC_TIME_STAMP "1546300800000000000"
#define MQTT_HMAC_NONCE 1024

esp_err_t mt_hmac_sha256_byte(const uint8_t *key, int key_size,
                              const uint8_t *id, int id_size,
                              uint8_t *time_stamp, int time_stamp_size,
                              uint32_t nonce, uint8_t *hmac);

unsigned char *mt_hmac_sha256_base64(const uint8_t *key, int key_size,
                                     const uint8_t *id, int id_size,
                                     uint8_t *time_stamp, int time_stamp_size,
                                     uint32_t nonce);

unsigned char *mt_hmac_sha256_mqtt(const uint8_t *key, int key_size,
                                   const uint8_t *id, int id_size);

char *mt_utils_int64_to_string(uint64_t src, uint8_t *size_out);

char *mt_utils_string_copy(char *str_in);

#endif
