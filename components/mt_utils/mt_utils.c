#include "stddef.h"
#include "string.h"

#include "crypto/base64.h"
#include "crypto/common.h"
#include "crypto/crypto.h"
#include "crypto/sha256.h"

#include "esp_log.h"

#include "mt_utils.h"
#include "mt_utils_login.h"

// static const char *TAG = "MT_UTILS";

int mt_hmac_sha256(const uint8_t *key, int key_size, const uint8_t *id,
                   int id_size, uint8_t *time_stamp, int time_stamp_size,
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

    sprintf(nonce_str, "%d", nonce);
    nonce_size = strlen(nonce_str);

    buf_size = id_size + time_stamp_size + nonce_size;
    buf = malloc(buf_size);
    memcpy((void *)buf, id, id_size);
    memcpy((void *)(buf + id_size), time_stamp, time_stamp_size);
    memcpy((void *)(buf + id_size + time_stamp_size), nonce_str, nonce_size);

    fast_hmac_sha256((uint8_t *)key_byte, 32, buf, buf_size, hmac);

    free(buf);
    return ESP_OK;
}

unsigned char *mt_hmac_sha256_mqtt(const uint8_t *key, int key_size,
                                   const uint8_t *id, int id_size)
{
    uint8_t hmac[32];
    size_t out_size;
    unsigned char *hmac_base64 = NULL;

    mt_hmac_sha256(key, key_size, id, id_size, (uint8_t *)MQTT_HMAC_TIME_STAMP,
                   sizeof(MQTT_HMAC_TIME_STAMP) - 1, (uint32_t)MQTT_HMAC_NONCE,
                   hmac);

    hmac_base64 = base64_encode(hmac, 32, &out_size);

    // set last char '\n' replaced with '0x00'
    if (out_size >= 1)
    {
        hmac_base64[out_size - 1] = 0x00;
    }

    return hmac_base64;
}

char *mt_utils_int64_to_string(uint64_t src, uint8_t *size_out)
{
    char des[64] = "";
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
