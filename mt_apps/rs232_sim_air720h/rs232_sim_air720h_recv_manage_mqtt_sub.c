#include "string.h"

#include "rs232_sim_air720h_flow.h"
#include "rs232_sim_air720h_recv_manage.h"
#include "rs232_sim_air720h_recv_manage_mqtt_sub.h"
#include "rs232_sim_air720h_utils.h"

#include "mt_mqtt_utils.h"

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
static int STAGE = 1;
static int MQTT_SUB_SIZE = 0;

#define MQTT_SUB_STAGE1_MAX_SIZE 256
static char STAGE1_BUF[MQTT_SUB_STAGE1_MAX_SIZE] = "";
static int STAGE1_SIZE = 0;
static char TOPIC[MQTT_SUB_STAGE1_MAX_SIZE] = "";

#define MQTT_SUB_STAGE2_MAX_SIZE 30
static char STAGE2_BUF[MQTT_SUB_STAGE2_MAX_SIZE] = "";
static int STAGE2_SIZE = 0;
static char STRSIZE[MQTT_SUB_STAGE2_MAX_SIZE] = "";
static int SIZE = 0;

#define MQTT_SUB_STAGE3_MAX_SIZE 1360
static char STAGE3_BUF[MQTT_SUB_STAGE3_MAX_SIZE] = "";
static int STAGE3_SIZE = 0;

// static func ===============================================================

static esp_err_t rs232_sim_air720h_recv_manage_mqtt_process_all(char *topic,
                                                                uint8_t *buf,
                                                                int size);

static void rs232_sim_air720h_recv_manage_mqtt_sub_reset() {
  STAGE = 1;
  STATE = false;
  MQTT_SUB_SIZE = 0;
  STAGE1_SIZE = 0;
  STAGE1_BUF[0] = '\0';
  TOPIC[0] = '\0';
  STAGE2_SIZE = 0;
  STAGE2_BUF[0] = '\0';
  SIZE = 0;
  STAGE3_SIZE = 0;
  STAGE3_BUF[0] = '\0';
}

// topic
static void rs232_sim_air720h_recv_manage_mqtt_sub_save_stage1(uint8_t byte) {
  STAGE1_SIZE++;

  if (STAGE1_SIZE >= MQTT_SUB_STAGE1_MAX_SIZE) {
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

  if (STAGE2_SIZE >= MQTT_SUB_STAGE2_MAX_SIZE) {
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
    if (STAGE2_SIZE < 7) {
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
    STRSIZE[STAGE2_SIZE - 6] = '\0';

    // size str to num
    SIZE = atoi(STRSIZE);
    if (SIZE <= 0) {
      ESP_LOGE(TAG, "%4d %s SIZE error:%d", __LINE__, __func__, SIZE);
      rs232_sim_air720h_recv_manage_mqtt_sub_reset();
      STAGE = -1;
      return;
    }

    // next stage
    STAGE = 3;
  }
}

// buf
static void rs232_sim_air720h_recv_manage_mqtt_sub_save_stage3(uint8_t byte) {
  STAGE3_SIZE++;

  if (STAGE3_SIZE >= MQTT_SUB_STAGE3_MAX_SIZE) {
    ESP_LOGE(TAG, "%4d %s STAGE3 buf reach max size", __LINE__, __func__);
    rs232_sim_air720h_recv_manage_mqtt_sub_reset();
    return;
  }

  if (STAGE3_SIZE < SIZE * 2) {
    STAGE3_BUF[STAGE3_SIZE - 1] = byte;

  } else {
    STAGE3_BUF[STAGE3_SIZE - 1] = byte;
    STAGE3_BUF[STAGE3_SIZE] = '\0';

    char *topic = NULL;
    int size = 0;
    uint8_t *buf = NULL;
    int buf_size = 0;
    topic = malloc(strlen(TOPIC) + 1);
    memcpy(topic, TOPIC, strlen(TOPIC) + 1);
    size = SIZE;
    buf = rs232_sim_air720h_utils_hex_to_byte((uint8_t *)STAGE3_BUF, SIZE * 2,
                                              &buf_size);
    if (buf == NULL) {
      ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_utils_byte_to_hex failed",
               __LINE__, __func__);
      goto EXIT;
    }

    rs232_sim_air720h_recv_manage_mqtt_process_all(topic, buf, size);

  EXIT:
    if (topic != NULL)
      free(topic);
    if (buf != NULL)
      free(buf);
    STAGE = 4;
    ESP_LOGI(TAG, "%4d %s mqtt_sub success, size:%d", __LINE__, __func__, SIZE);
  }
}

// static func ===============================================================

static esp_err_t rs232_sim_air720h_recv_manage_mqtt_process_all(char *topic,
                                                                uint8_t *buf,
                                                                int size) {
  esp_err_t err = ESP_OK;
  char *path = NULL;

  ESP_LOGI(TAG, "%4d %s get mqtt msg, topic:%s", __LINE__, __func__, topic);
  path = mt_mqtt_utils_get_path_from_topic(topic);
  if (path == NULL) {
    ESP_LOGE(TAG, "%4d %s get unexcept topic:%s", __LINE__, __func__, topic);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // TODO(ZH)
  /*
  if (strcmp(path, "proxy") == 0) {
    ESP_LOGI(TAG, "%4d %s get proxy message", __LINE__, __func__);
    mt_module_mqtt_proxy_process(topic, buf, size);
    goto EXIT;
  }*/

  if (strcmp(path, "flow_channel") == 0) {
    ESP_LOGI(TAG, "%4d %s get flow message", __LINE__, __func__);
    err = rs232_sim_air720h_flow_process_all(topic, buf, size);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_flow_process_all failed",
               __LINE__, __func__);
    }
    goto EXIT;
  }

  ESP_LOGE(TAG, "%4d %s unexcept path recieve:%s", __LINE__, __func__, path);
  err = ESP_ERR_INVALID_ARG;

EXIT:
  if (path != NULL)
    free(path);
  return err;
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

esp_err_t rs232_sim_air720h_recv_manage_process_mqtt_sub(uint8_t byte) {
  esp_err_t err = ESP_OK;

  if (STAGE == -1) {
    ESP_LOGE(TAG, "%4d %s ", __LINE__, __func__);
    rs232_sim_air720h_recv_manage_mqtt_sub_reset();
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (STAGE == 1) {
    rs232_sim_air720h_recv_manage_mqtt_sub_save_stage1(byte);
    goto EXIT;
  }

  if (STAGE == 2) {
    rs232_sim_air720h_recv_manage_mqtt_sub_save_stage2(byte);
    goto EXIT;
  }

  if (STAGE == 3) {
    rs232_sim_air720h_recv_manage_mqtt_sub_save_stage3(byte);
    goto EXIT;
  }

  if (STAGE == 4) {
    rs232_sim_air720h_recv_manage_reset_cache();
    STATE = false;
    goto EXIT;
  }

EXIT:
  UPDATE_TIME = esp_timer_get_time() / 1000000;

  return err;
}
