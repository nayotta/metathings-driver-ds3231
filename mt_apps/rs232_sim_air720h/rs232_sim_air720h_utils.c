#include "rs232_sim_air720h_utils.h"

// static define ==============================================================

static const char *TAG = "rs232_sim_air720_utils.h";

static char *HEAD_STR =
    "Content-Type:application/json\\r\\nAccept:application/json";
#define AIR720H_UTILS_HEAD_MAX_SIZE 200

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

  sprintf(temp_head, "%s\\r\\nAuthorization:Bearer %s", HEAD_STR, token);
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
          "%s\\r\\nAuthorization:Bearer %s\\r\\nMT-Module-Session:%llu",
          HEAD_STR, token, session);
  head_out_size = strlen(temp_head) + 1;
  head_out = malloc(head_out_size);
  memcpy(head_out, temp_head, head_out_size);

EXIT:
  return head_out;
}
