#include "rs232_sim_air720h_recv_manage_http_action.h"

// static define ==============================================================

static const char *TAG = "rs232_sim_air720_recv_manage_http_action";

static bool ACTION_STATE = false;
static int ACTION_METHOD = 0;
static int ACTION_CODE = 0;
static int ACTION_RES_SIZE = 0;

#define ACTION_METHOD_MAX_SIZE 10
static char ACTION_METHOD_STR[ACTION_METHOD_MAX_SIZE] = "";
#define ACTION_CODE_MAX_SIZE 10
static char ACTION_CODE_STR[ACTION_CODE_MAX_SIZE] = "";
#define ACTION_RES_SIZE_MAX_SIZE 10
static char ACTION_RES_SIZE_STR[ACTION_RES_SIZE_MAX_SIZE] = "";

// static func ================================================================

// global func================================================================

void rs232_sim_air720h_recv_manage_reset_http_action() {
  ACTION_STATE = false;
  ACTION_METHOD = 0;
  ACTION_CODE = 0;
  ACTION_RES_SIZE = 0;
  ACTION_METHOD_STR[0] = '\0';
  ACTION_CODE_STR[0] = '\0';
  ACTION_RES_SIZE_STR[0] = '\0';
}

void rs232_sim_air720h_recv_manage_get_http_get_action_state(
    rs232_sim_air720h_recv_manage_http_action *action) {
  action->code = ACTION_CODE;
  action->method = ACTION_METHOD;
  action->state = ACTION_STATE;
  action->size = ACTION_RES_SIZE;
}

esp_err_t rs232_sim_air720h_recv_manage_get_http_get_action_finish() {
  if (ACTION_STATE == true) {
    return ESP_OK;
  } else {
    return ESP_ERR_INVALID_RESPONSE;
  }
}

void rs232_sim_air720h_recv_manage_process_action(char *buf_in) {
  int offset = 12;
  int count = 0;
  int buf_size = 0;

  if (buf_in == NULL) {
    ESP_LOGE(TAG, "%4d %s buf NULL", __LINE__, __func__);
    return;
  }

  buf_size = strlen(buf_in);
  if (buf_size < offset + 3) {
    ESP_LOGE(TAG, "%4d %s buf size error:%d", __LINE__, __func__, buf_size);
    return;
  }

  // stage 1 method
  count = 0;
  while (buf_in[offset] != ',' && offset <= buf_size &&
         count < ACTION_METHOD_MAX_SIZE - 1) {
    ACTION_METHOD_STR[count] = buf_in[offset];
    count++;
    offset++;
  }
  ACTION_METHOD_STR[offset + 1] = '\0';
  offset++;
  ACTION_METHOD = atoi(ACTION_METHOD_STR);

  // statge 2 code
  count = 0;
  while (buf_in[offset] != ',' && offset <= buf_size &&
         count < ACTION_CODE_MAX_SIZE - 1) {
    ACTION_CODE_STR[count] = buf_in[offset];
    count++;
    offset++;
  }
  ACTION_CODE_STR[offset + 1] = '\0';
  offset++;
  ACTION_CODE = atoi(ACTION_CODE_STR);

  // statge 3 res size
  count = 0;
  while (offset <= buf_size && count < ACTION_RES_SIZE_MAX_SIZE - 1) {
    ACTION_RES_SIZE_STR[count] = buf_in[offset];
    count++;
    offset++;
  }
  ACTION_RES_SIZE_STR[offset + 1] = '\0';
  offset++;
  ACTION_RES_SIZE = atoi(ACTION_RES_SIZE_STR);

  // finish
  ACTION_STATE = true;
}

esp_err_t rs232_sim_air720h_recv_manage_http_action_check_post(int code) {
  esp_err_t err = ESP_OK;

  rs232_sim_air720h_recv_manage_http_action action = {false, 0, 0, 0};
  rs232_sim_air720h_recv_manage_get_http_get_action_state(&action);

  // check state
  if (action.state != true) {
    ESP_LOGE(TAG, "%4d %s action failed", __LINE__, __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // check method
  if (action.method != rs232_sim_air720h_recv_manage_http_action_method_post) {
    ESP_LOGE(TAG, "%4d %s method error:%d", __LINE__, __func__, action.method);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // check code
  if (action.code != code) {
    ESP_LOGE(TAG, "%4d %s code error require:%d get:%d", __LINE__, __func__,
             code, action.code);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return err;
}
