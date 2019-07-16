#include "stddef.h"
#include "string.h"

#include "crypto/base64.h"
#include "crypto/common.h"
#include "crypto/crypto.h"
#include "crypto/sha256.h"

#include "esp_err.h"
#include "esp_log.h"

#include "mt_utils.h"
#include "mt_utils_login.h"

static const char *TAG = "MT_UTILS";

esp_err_t mt_hmac_sha256_byte(const uint8_t *key, int key_size,
                              const uint8_t *id, int id_size,
                              uint8_t *time_stamp, int time_stamp_size,
                              uint32_t nonce, uint8_t hmac[32])
{
    // key use 32byte if short pad with oxff, if long drop end
    unsigned char *key_byte = NULL;
    size_t key_byte_size;
    uint8_t fmt_key[32] = {0x00};
    uint8_t *buf;
    int buf_size;
    uint8_t nonce_str[32];
    int nonce_size = 0;

    key_byte = base64_decode(key, key_size, &key_byte_size);

    if (key_byte_size >= 32)
    {
        memcpy((void *)fmt_key, key, 32);
    }
    else
    {
        memcpy((void *)fmt_key, key, key_byte_size);
        for (int i = 0; i < (32 - key_byte_size); i++)
        {
            fmt_key[key_byte_size + i] = 0xff;
        }
    }

    sprintf((char *)nonce_str, "%u", nonce);
    nonce_size = strlen((char *)nonce_str);
    ESP_LOGW(TAG,
             "id_size:%d, id:%s, time_stamp_size:%d, "
             "time_stamp:%s, nonce_size=%d, nonce=%s",
             id_size, id, time_stamp_size, time_stamp, nonce_size, nonce_str);

    buf_size = id_size + time_stamp_size + nonce_size;
    buf = malloc(buf_size);
    memcpy((void *)buf, id, id_size);
    memcpy((void *)(buf + id_size), time_stamp, time_stamp_size);
    memcpy((void *)(buf + id_size + time_stamp_size), nonce_str, nonce_size);

    fast_hmac_sha256((uint8_t *)key_byte, 32, buf, buf_size, hmac);

    free(buf);
    return ESP_OK;
}

unsigned char *mt_hmac_sha256_base64(const uint8_t *key, int key_size,
                                     const uint8_t *id, int id_size,
                                     uint8_t *time_stamp, int time_stamp_size,
                                     uint32_t nonce)
{
    esp_err_t err = ESP_OK;
    uint8_t hmac[32];
    unsigned char *hmac_base64 = NULL;
    size_t out_size = 0;

    err = mt_hmac_sha256_byte(key, key_size, id, id_size, time_stamp,
                              time_stamp_size, nonce, hmac);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%4d %s mt_hmac_sha256 error", __LINE__, __func__);
        return NULL;
    }

    hmac_base64 = base64_encode(hmac, 32, &out_size);

    // set last char '\n' replaced with '0x00'
    if (out_size >= 1)
    {
        hmac_base64[out_size - 1] = 0x00;
    }

    return hmac_base64;
}

unsigned char *mt_hmac_sha256_mqtt(const uint8_t *key, int key_size,
                                   const uint8_t *id, int id_size)
{
    return mt_hmac_sha256_base64(
        key, key_size, id, id_size, (uint8_t *)MQTT_HMAC_TIME_STAMP,
        sizeof(MQTT_HMAC_TIME_STAMP) - 1, (uint32_t)MQTT_HMAC_NONCE);
}

char *mt_utils_int64_to_string(uint64_t src, uint8_t *size_out)
{
    char *des = malloc(64);
    char *ret = NULL;

    sprintf(des, "%llu", src);
    if (des != NULL)
    {
        *size_out = (uint8_t)strlen(des);
    }

    if (*size_out > 0)
    {
        ret = malloc(strlen(des));
        memcpy(ret, des, *size_out);
    }
    else
    {
        return NULL;
    }

    return des;
}

char *mt_utils_string_copy(char *str_in)
{
    char *str_out = NULL;

    str_out = malloc(strlen(str_in) + 1);
    str_out[strlen(str_in)] = '\0';
    memcpy(str_out, str_in, strlen(str_in));

    return str_out;
}
