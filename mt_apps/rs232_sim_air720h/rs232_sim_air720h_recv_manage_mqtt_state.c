#include "string.h"

#include "rs232_sim_air720h_flow.h"
#include "rs232_sim_air720h_recv_manage.h"
#include "rs232_sim_air720h_recv_manage_mqtt_state.h"
#include "rs232_sim_air720h_recv_manage_mqtt_sub.h"
#include "rs232_sim_air720h_utils.h"

#include "mt_mqtt_utils.h"

#include "esp_timer.h"

// static define ==============================================================

static const char *TAG = "rs232_sim_air720_recv_manage_mqtt_state";

static int32_t STATE_CODE = -1;
static bool STATE_STATE = false;
#define RS232_SIM_AIR720H_RECV_MANAGE_MQTT_STATE_MAX_SIZE (20)
static char STATE_STR[RS232_SIM_AIR720H_RECV_MANAGE_MQTT_STATE_MAX_SIZE] = "";

// help func =================================================================

void rs232_sim_air720h_recv_manage_mqtt_reset() {
  STATE_STATE = false;
  STATE_CODE = -1;
  STATE_STR[0] = '\0';
}

// global func ===============================================================

esp_err_t rs232_sim_air720h_recv_manage_process_mqtt_state(char *buf) {
  int count = 0;

  if (strlen(buf) < 12 ||
      strlen(buf) - 12 > RS232_SIM_AIR720H_RECV_MANAGE_MQTT_STATE_MAX_SIZE) {
    ESP_LOGE(TAG, "%4d %s buf size error:%d", __LINE__, __func__, strlen(buf));
    return ESP_ERR_INVALID_ARG;
  }

  for (int i = 11; i < strlen(buf); i++) {
    STATE_STR[count] = buf[i];
    count++;
  }
  STATE_STR[count] = '\0';

  STATE_CODE = atoi(STATE_STR);
  STATE_STATE = true;

  return ESP_OK;
}

int32_t rs232_sim_air720h_recv_manage_get_mqtt_state() { return STATE_CODE; }

esp_err_t rs232_sim_air720h_recv_manage_get_mqtt_state_finish() {
  if (STATE_STATE == true) {
    return ESP_OK;
  } else {
    return ESP_ERR_INVALID_RESPONSE;
  }
}
