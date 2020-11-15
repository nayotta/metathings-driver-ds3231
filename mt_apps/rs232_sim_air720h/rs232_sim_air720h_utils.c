#include "rs232_sim_air720h_utils.h"

// static define ==============================================================

static const char *TAG = "rs232_sim_air720_utils.h";

static const char CHHEX[] = "0123456789ABCDEF";

static char *HEAD_STR =
    "Content-Type:application/json \\r\\nAccept:application/json ";
#define AIR720H_UTILS_HEAD_MAX_SIZE 200

// static func ================================================================

static int char_to_int(char target) {
  if ((target - '0') >= 0 && (target - '0') <= 9) {
    return target - '0';
  } else if ((target - 'A') >= 0 && (target - 'A') <= 5) {
    return 10 + target - 'A';
  } else {
    return -1;
  }
}

// global func ================================================================

char *rs232_sim_air720h_utils_get_head() {
  char *head = NULL;
  int head_size = 0;

  head_size = strlen(HEAD_STR) + 1;
  head = malloc(head_size);
  memcpy(head, HEAD_STR, head_size);

  return head;
}

char *rs232_sim_air720h_utils_get_head_with_token(char *token) {
  char *head_out = NULL;
  char temp_head[AIR720H_UTILS_HEAD_MAX_SIZE] = "";
  int head_out_size = 0;

  if (token == NULL) {
    ESP_LOGE(TAG, "%4d %s token NULL", __LINE__, __func__);
    goto EXIT;
  }

  sprintf(temp_head, "%s\\nAuthorization:Bearer %s", HEAD_STR, token);
  head_out_size = strlen(temp_head) + 1;
  head_out = malloc(head_out_size);
  memcpy(head_out, temp_head, head_out_size);

EXIT:
  return head_out;
}

char *
rs232_sim_air720h_utils_get_head_with_token_and_session(char *token,
                                                        uint64_t session) {
  char *head_out = NULL;
  char temp_head[AIR720H_UTILS_HEAD_MAX_SIZE] = "";
  int head_out_size = 0;

  if (token == NULL) {
    ESP_LOGE(TAG, "%4d %s token NULL", __LINE__, __func__);
    goto EXIT;
  }

  if (session == 0) {
    ESP_LOGE(TAG, "%4d %s session zero", __LINE__, __func__);
    goto EXIT;
  }

  sprintf(temp_head,
          "%s\\nAuthorization:Bearer %s\\nMT-Module-Session:%llu",
          HEAD_STR, token, session);
  head_out_size = strlen(temp_head) + 1;
  head_out = malloc(head_out_size);
  memcpy(head_out, temp_head, head_out_size);

EXIT:
  return head_out;
}

uint8_t *rs232_sim_air720h_utils_hex_to_byte(uint8_t *buf_in, int size_in,
                                             int *size_out) {
  esp_err_t err = ESP_OK;
  uint8_t *buf_out = NULL;

  if (size_in % 2 != 0 || size_in <= 0) {
    ESP_LOGE(TAG, "%4d %s error len:%d", __LINE__, __func__, size_in);
    return NULL;
  }

  *size_out = size_in / 2;

  buf_out = malloc(*size_out);

  for (int i = 0; i < *size_out; i++) {
    int head = char_to_int((char)buf_in[2 * i]);
    if (head < 0) {
      ESP_LOGE(TAG, "%4d %s get error hex byte", __LINE__, __func__);
      err = ESP_ERR_INVALID_RESPONSE;
      goto EXIT;
    }

    int end = char_to_int((char)buf_in[2 * i + 1]);
    if (end < 0) {
      ESP_LOGE(TAG, "%4d %s get error hex byte", __LINE__, __func__);
      err = ESP_ERR_INVALID_RESPONSE;
      goto EXIT;
    }

    buf_out[i] = head << 4 | end;
  }
EXIT:
  if (err != ESP_OK) {
    if (buf_out != NULL) {
      free(buf_out);
      buf_out = NULL;
    }
  }

  return buf_out;
}

char *rs232_sim_air720h_utils_byte_to_hex(uint8_t *buf_in, int size_in) {
  char *ret_buf = malloc(2 * size_in + 1);

  for (int i = 0; i < size_in; i++) {
    ret_buf[2 * i] = CHHEX[buf_in[i] >> 4];
    ret_buf[2 * i + 1] = CHHEX[buf_in[i] & 0x0F];
  }
  ret_buf[2 * size_in] = '\0';

  return ret_buf;
}
