#include "rs232_sim_air720h_recv_manage_mqtt_sub.h"

#include "esp_timer.h"

// static define ==============================================================

static const char *TAG = "rs232_sim_air720_recv_manage_mqtt_sub";

static const char *STR_MQTT_BYTE = " BYTE";
static int TIMEOUT = 300; // 300ms
static int64_t UPDATE_TIME = 0;
static bool STATE = false;
/*
    stage -1 error
    stage 1  topic
    stage 2  size
    stage 3  msg buf
    stage 4  finished
*/
static int TOTOAL_STATGE = 4;
static int STAGE = 1;
static int MQTT_SUB_SIZE = 0;

static const int STAGE1_MAX_SIZE = 256;
static char STAGE1_BUF[STAGE1_MAX_SIZE] = "";
static int STAGE1_SIZE = 0;
static char TOPIC[STAGE1_MAX_SIZE] = "";

static const int STAGE2_MAX_SIZE = 15;
static char STAGE2_BUF[STAGE2_MAX_SIZE] = "";
static int STAGE2_SIZE = 0;
static char STRSIZE[STAGE2_MAX_SIZE] = "";
static int SIZE = 0;

static const int STAGE3_MAX_SIZE = 1360;
static char STAGE3_BUF[STAGE3_MAX_SIZE] = "";
static int STAGE3_SIZE = 0;

// static func ===============================================================

static void rs232_sim_air720h_recv_manage_mqtt_sub_reset() {
  STATE = 1;
  STATE = false;
  MQTT_SUB_SIZE = 0;
  STAGE1_SIZE = 0;
  STAGE1_BUF[0] = '\0';
  STAGE2_SIZE = 0;
  STAGE2_BUF[0] = '\0';
  STAGE3_SIZE = 0;
  STAGE3_BUF[0] = '\0';
}

// topic
static void rs232_sim_air720h_recv_manage_mqtt_sub_save_stage1(uint8_t byte) {
  STAGE1_SIZE++;

  if (STAGE1_SIZE > STAGE1_MAX_SIZE) {
    ESP_LOGE(TAG, "%4d %s STAGE1 buf reach max size", __LINE__, __func__);
    rs232_sim_air720h_recv_manage_mqtt_sub_reset();
    STAGE = -1;
    return;
  }

  if (byte != ',') {
    STAGE1_BUF[STAGE1_SIZE - 1] = byte;
  } else {
    STAGE1_BUF[STAGE1_SIZE - 1] = '\0';

    // check topic size
    if (STAGE1_SIZE < 4) {
      ESP_LOGE(TAG, "%4d %s topic size too little", __LINE__, __func__);
      rs232_sim_air720h_recv_manage_mqtt_sub_reset();
      STAGE = -1;
      return;
    }

    // check topic byte 1
    if (STAGE1_BUF[0] != '\"') {
      ESP_LOGE(TAG, "%4d %s topic byte 0:%c not \"", __LINE__, __func__,
               STAGE1_BUF[0]);
      rs232_sim_air720h_recv_manage_mqtt_sub_reset();
      STAGE = -1;
      return;
    }

    // check topic byte last
    if (STAGE1_BUF[STAGE1_SIZE - 2] != '\"') {
      ESP_LOGE(TAG, "%4d %s topic byte last:%c not \"", __LINE__, __func__,
               STAGE1_BUF[STAGE1_SIZE - 2]);
      rs232_sim_air720h_recv_manage_mqtt_sub_reset();
      STAGE = -1;
      return;
    }

    // copy topic
    memcpy(TOPIC, STAGE1_BUF + 1, STAGE1_SIZE - 3);
    TOPIC[STAGE1_SIZE - 3] = '\0';

    // next stage
    STAGE = 2;
  }
}

// size
static void rs232_sim_air720h_recv_manage_mqtt_sub_save_stage2(uint8_t byte) {
  STAGE2_SIZE++;

  if (STAGE2_SIZE > STAGE2_MAX_SIZE) {
    ESP_LOGE(TAG, "%4d %s STAGE2 buf reach max size", __LINE__, __func__);
    rs232_sim_air720h_recv_manage_mqtt_sub_reset();
    STAGE = -1;
    return;
  }

  if (byte != ',') {
    STAGE2_BUF[STAGE2_SIZE - 1] = byte;
  } else {
    STAGE2_BUF[STAGE2_SIZE - 1] = '\0';

    // check size size
    if (STAGE1_SIZE < 7) {
      ESP_LOGE(TAG, "%4d %s size too little", __LINE__, __func__);
      rs232_sim_air720h_recv_manage_mqtt_sub_reset();
      STAGE = -1;
      return;
    }

    // check size byte last
    if (strstr(STAGE2_BUF + STAGE2_SIZE - 6, STR_MQTT_BYTE)) {
      ESP_LOGE(TAG, "%4d %s topic byte last:%s not %s", __LINE__, __func__,
               STAGE2_BUF + STAGE2_SIZE - 6, STR_MQTT_BYTE);
      rs232_sim_air720h_recv_manage_mqtt_sub_reset();
      STAGE = -1;
      return;
    }

    // copy size str
    memcpy(STRSIZE, STAGE2_BUF, STAGE2_SIZE - 6);
    STRSIZE[STAGE1_SIZE - 6] = '\0';

    // size str to num
    SIZE = atoi(STRSIZE);
    if (SIZE <= 0) {
      ESP_LOGE(TAG, "%4d %s SIZE error:%d", __LINE__, __func__, SIZE);
      rs232_sim_air720h_recv_manage_mqtt_sub_reset();
      STAGE = -1;
      return;
    }

    // next stage
    STAGE = 2;
  }
}

// buf
static void rs232_sim_air720h_recv_manage_mqtt_sub_save_stage3(uint8_t byte) {
  STAGE3_SIZE++;

  if (STAGE3_SIZE > STAGE3_MAX_SIZE) {
    ESP_LOGE(TAG, "%4d %s STAGE3 buf reach max size", __LINE__, __func__);
    rs232_sim_air720h_recv_manage_mqtt_sub_reset();
    return;
  }

  if (STAGE3_SIZE < SIZE) {
    STAGE3_BUF[STAGE3_SIZE - 1] = byte;

  } else {
    STAGE3_BUF[STAGE3_SIZE - 1] = byte;
    STAGE3_BUF[STAGE3_SIZE] = '\0';
    STATE = false;
    STAGE = 4;
    ESP_LOGI(TAG, "%4d %s mqtt_sub success, size:%d", __LINE__, __func__, SIZE);
  }
}

// global func ===============================================================

bool rs232_sim_air720h_recv_manage_get_mqtt_sub_state() {
  int64_t now = esp_timer_get_time() / 1000000;

  if (now - UPDATE_TIME > TIMEOUT) {
    STATE = false;
  }

  return STATE;
}

void rs232_sim_air720h_recv_manage_set_mqtt_sub_state(bool state_in) {
  UPDATE_TIME = esp_timer_get_time() / 1000000;
  if (state_in == true) {
    rs232_sim_air720h_recv_manage_mqtt_sub_reset();
  }
  STATE = state_in;
}

esp_err_t rs232_sim_air720h_recv_manage_process_mqtt_pub(uint8_t byte) {
  esp_err_t err = ESP_OK;

  if (STAGE == -1) {
    ESP_LOGE(TAG, "%4d %s ", __LINE__, __func__);
    rs232_sim_air720h_recv_manage_mqtt_sub_reset();
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (STAGE == 1) {
    rs232_sim_air720h_recv_manage_mqtt_sub_save_stage1(byte);
  }

  if (STAGE == 2) {
    rs232_sim_air720h_recv_manage_mqtt_sub_save_stage2(byte);
  }

  if (STAGE == 3) {
    rs232_sim_air720h_recv_manage_mqtt_sub_save_stage3(byte);
  }

  if (STAGE == 4) {
    STATE = false;
  }

  UPDATE_TIME = esp_timer_get_time() / 1000000;

  return err;
}
