#include "rs232_sim_air720h_recv_manage_http_read.h"

#include "esp_timer.h"

// static define ==============================================================

static const char *TAG = "rs232_sim_air720_recv_manage_http_read";

static int TIMEOUT = 300; // 300 ms
static int64_t UPDATE_TIME = 0;
static bool STATE = false;
static int TOTAL_STAGE = 3; // -1 error 1 get size 2 get buf 3 finished
static int STAGE = 1;
static int HTTP_READ_SIZE = 0;
static const int STAGE1_MAX_SIZE = 10;
static char STAGE1_BUF[STAGE1_MAX_SIZE] = "";
static int STAGE1_SIZE = 0;
static const int STAGE2_MAX_SIZE = 1024;
static char STAGE2_BUF[STAGE2_MAX_SIZE] = "";
static int STAGE2_SIZE = 0;

// static func ===============================================================

static void rs232_sim_air720h_recv_manage_http_read_save_stage1(uint8_t byte) {
  STAGE1_SIZE++;

  if (STAGE1_SIZE > STAGE1_MAX_SIZE) {
    ESP_LOGE(TAG, "%4d %s STAGE1 buf reach max size", __LINE__, __func__);
    rs232_sim_air720h_recv_manage_http_read_reset();
    STAGE = -1;
    return;
  }

  if (byte != '\n') {
    STAGE1_BUF[STAGE1_SIZE - 1] = byte;
  } else {
    STAGE1_BUF[STAGE1_SIZE - 1] = '\0';
    HTTP_READ_SIZE = atoi(STAGE1_BUF);
    if (HTTP_READ_SIZE <= 0) {
      ESP_LOGE(TAG, "%4d %s HTTP_READ_SIZE error:%d", __LINE__, __func__,
               HTTP_READ_SIZE);
      rs232_sim_air720h_recv_manage_http_read_reset();
      STAGE = -1;
      return;
    } else {
      STAGE = 2;
    }
  }
}

static void rs232_sim_air720h_recv_manage_http_read_save_stage2(uint8_t byte) {
  STAGE2_SIZE++;

  if (STAGE2_SIZE > STAGE2_MAX_SIZE) {
    ESP_LOGE(TAG, "%4d %s STAGE2 buf reach max size", __LINE__, __func__);
    rs232_sim_air720h_recv_manage_http_read_reset();
    return;
  }

  if (STAGE2_SIZE < HTTP_READ_SIZE) {
    STAGE2_BUF[STAGE2_SIZE - 1] = byte;

  } else {
    STAGE2_BUF[STAGE2_SIZE - 1] = byte;
    STAGE2_BUF[STAGE2_SIZE] = '\0';
    STATE = false;
    STAGE = 3;
    ESP_LOGI(TAG, "%4d %s http_read success, size:%d", __LINE__, __func__,
             HTTP_READ_SIZE);
  }
}

// global func ================================================================

void rs232_sim_air720h_recv_manage_http_read_reset() {
  STATE = 1;
  STATE = false;
  HTTP_READ_SIZE = 0;
  STAGE1_SIZE = 0;
  STAGE1_BUF[0] = '\0';
  STAGE2_SIZE = 0;
  STAGE2_BUF[0] = '\0';
}

bool rs232_sim_air720h_recv_manage_get_http_read_state() {
  int64_t now = esp_timer_get_time() / 1000000;

  if (now - UPDATE_TIME > TIMEOUT) {
    STATE = false;
  }

  return STATE;
}

void rs232_sim_air720h_recv_manage_set_http_read_state(bool state_in) {
  UPDATE_TIME = esp_timer_get_time() / 1000000;
  if (state_in == true) {
    rs232_sim_air720h_recv_manage_http_read_reset();
  }
  STATE = state_in;
}

esp_err_t rs232_sim_air720h_recv_manage_process_http_read(uint8_t byte) {
  esp_err_t err = ESP_OK;

  if (STAGE == -1) {
    ESP_LOGE(TAG, "%4d %s ", __LINE__, __func__);
    rs232_sim_air720h_recv_manage_http_read_reset();
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (STAGE == 1) {
    rs232_sim_air720h_recv_manage_http_read_save_stage1(byte);
  }

  if (STAGE == 2) {
    rs232_sim_air720h_recv_manage_http_read_save_stage2(byte);
  }

  if (STAGE == 3) {
    STATE = false;
  }

  UPDATE_TIME = esp_timer_get_time() / 1000000;

  return err;
}

esp_err_t rs232_sim_air720h_recv_manage_get_http_read_finish() {
  if (STAGE == TOTAL_STAGE) {
    return ESP_OK;
  } else {
    return ESP_ERR_INVALID_RESPONSE;
  }
}

char *rs232_sim_air720h_recv_manage_get_http_read_res() {
  char *buf_out = NULL;

  if (STAGE2_SIZE == 0) {
    return NULL;
  }

  buf_out = malloc(STAGE2_SIZE);
  memcpy(buf_out, STAGE2_BUF, STAGE2_SIZE);
  return buf_out;
}
