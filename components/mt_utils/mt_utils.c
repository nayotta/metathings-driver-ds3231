#include "stddef.h"
#include "string.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"

#include "mbedtls/md.h"

#include "mt_utils.h"
#include "mt_utils_base64.h"
#include "mt_utils_login.h"

static const char *TAG = "MT_UTILS";

esp_err_t mt_hmac_sha256_byte(const uint8_t *key, int key_size,
                              const uint8_t *id, int id_size,
                              uint8_t *time_stamp, int time_stamp_size,
                              uint32_t nonce, uint8_t hmac[32]) {

  // key use 32byte if short pad with oxff, if long drop end
  unsigned char *key_byte = NULL;
  unsigned int key_byte_size;
  uint8_t fmt_key[32] = "";
  uint8_t *buf = NULL;
  int buf_size;
  uint8_t nonce_str[32] = "";
  int nonce_size = 0;

  key_byte = malloc(BASE64_DECODE_OUT_SIZE(key_size));
  key_byte_size = mt_utils_base64_decode((char *)key, key_size, key_byte);

  if (key_byte_size >= 32) {
    memcpy((void *)fmt_key, key, 32);
  } else {
    memcpy((void *)fmt_key, key, key_byte_size);
    for (int i = 0; i < (32 - key_byte_size); i++) {
      fmt_key[key_byte_size + i] = 0xff;
    }
  }

  sprintf((char *)nonce_str, "%u", nonce);
  nonce_size = strlen((char *)nonce_str);

  buf_size = id_size + time_stamp_size + nonce_size;
  buf = malloc(buf_size);
  memcpy((void *)buf, id, id_size);
  memcpy((void *)(buf + id_size), time_stamp, time_stamp_size);
  memcpy((void *)(buf + id_size + time_stamp_size), nonce_str, nonce_size);

  mbedtls_md_context_t ctx;
  const mbedtls_md_info_t *info;

  mbedtls_md_init(&ctx);
  info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);

  mbedtls_md_setup(&ctx, info, 1);

  mbedtls_md_hmac_starts(&ctx, (unsigned char *)key_byte, key_byte_size);
  mbedtls_md_hmac_update(&ctx, (unsigned char *)buf, buf_size);
  mbedtls_md_hmac_finish(&ctx, hmac);

  mbedtls_md_free(&ctx);

  free(buf);
  if (key_byte != NULL)
    free(key_byte);
  return ESP_OK;
}

unsigned char *mt_hmac_sha256_base64(const uint8_t *key, int key_size,
                                     const uint8_t *id, int id_size,
                                     uint8_t *time_stamp, int time_stamp_size,
                                     uint32_t nonce) {
  esp_err_t err = ESP_OK;
  uint8_t hmac[32];
  unsigned char *hmac_base64 = NULL;
  unsigned int out_size = 0;

  err = mt_hmac_sha256_byte(key, key_size, id, id_size, time_stamp,
                            time_stamp_size, nonce, hmac);

  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_hmac_sha256 error", __LINE__, __func__);
    return NULL;
  }

  hmac_base64 = malloc(BASE64_ENCODE_OUT_SIZE(32));
  out_size =
      mt_utils_base64_encode((unsigned char *)hmac, 32, (char *)hmac_base64);

  if (out_size == 0) {
    ESP_LOGE(TAG, "%4d %s out_size zero", __LINE__, __func__);
    return NULL;
  }

  return hmac_base64;
}

unsigned char *mt_hmac_sha256_mqtt(const uint8_t *key, int key_size,
                                   const uint8_t *id, int id_size) {
  return mt_hmac_sha256_base64(
      key, key_size, id, id_size, (uint8_t *)MQTT_HMAC_TIME_STAMP,
      sizeof(MQTT_HMAC_TIME_STAMP) - 1, (uint32_t)MQTT_HMAC_NONCE);
}

char *mt_utils_int64_to_string(uint64_t src) {
  char *des = malloc(64);
  char *ret = NULL;
  int size_out = 0;

  sprintf(des, "%llu", src);
  if (des != NULL) {
    size_out = (uint8_t)strlen(des) + 1;
  }

  if (size_out > 0) {
    ret = malloc(strlen(des) + 1);
    ret[strlen(des)] = '\0';
    memcpy(ret, des, strlen(des));
  } else {
    return NULL;
  }
  if (des != NULL)
    free(des);

  return ret;
}

char *mt_utils_get_random_client_id(int id_size) {
  char range[] =
      "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char *buf_out = NULL;

  if (id_size > 0) {
    buf_out = malloc(id_size + 1);
    buf_out[id_size] = '\0';
  } else {
    ESP_LOGE(TAG, "unsupport id size:%d", id_size);
    return NULL;
  }

  for (int i = 0; i < id_size; i++) {
    buf_out[i] = range[esp_random() % (strlen(range))];
  }

  return buf_out;
}
