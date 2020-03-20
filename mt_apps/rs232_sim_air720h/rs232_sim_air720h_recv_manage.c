#include "string.h"

#include "rs232_dev.h"
#include "rs232_sim_air720h_recv_manage.h"

#include "rs232_sim_air720h_mqtt.h"
#include "rs232_sim_air720h_recv_manage_http_action.h"
#include "rs232_sim_air720h_recv_manage_http_read.h"
#include "rs232_sim_air720h_recv_manage_mqtt_sub.h"

// static define ==============================================================

static const char *TAG = "rs232_sim_air720_recv_manage";

#define RS232_SIM_AIR720H_MAX_BUF_SIZE (1024)
static uint8_t RECV_BUF[RS232_SIM_AIR720H_MAX_BUF_SIZE];
static int RECV_SIZE = 0;

static char *STR_OK = "OK";
static char *STR_ERROR = "ERROR";
static char *STR_DOWNLOAD = "DOWNLOAD";
static char *STR_CONNECT_OK = "CONNECT OK";
static char *STR_NET_OK = "+CGATT: 1";
static char *STR_MQTT_CONN_OK = "CONNACK OK";
static char *STR_MQTT_SUB_OK = "SUBACK";
static char *STR_MQTT_CLOSE_OK = "CLOSE";

static char *STR_HTTP_READ = "+HTTPREAD: ";
static char *STR_HTTP_ACTION = "+HTTPACTION:";
static char *STR_MQTT_SUB = "+MSUB: ";

static rs232_sim_air720h_ack ACK = {false, false, false, false,
                                    false, false, false, false};

// static func ================================================================

static void rs232_sim_air720h_recv_manage_parse_msg() {
  if (RECV_SIZE <= 2) {
    goto EXIT;
  }

  // ESP_LOGI(TAG, "%4d %s echo back msg %s", __LINE__, __func__, RECV_BUF);

  if (strstr((char *)RECV_BUF, "AT+")) {
    goto EXIT;
  }

  if (strstr((char *)RECV_BUF, STR_OK)) {
    ACK.ack_ok = true;
    goto EXIT;
  }

  if (strstr((char *)RECV_BUF, STR_ERROR)) {
    ACK.ack_error = true;
    goto EXIT;
  }

  if (strstr((char *)RECV_BUF, STR_DOWNLOAD)) {
    ACK.ack_download = true;
    goto EXIT;
  }

  if (strstr((char *)RECV_BUF, STR_CONNECT_OK)) {
    ACK.ack_connect_ok = true;
    goto EXIT;
  }

  if (strstr((char *)RECV_BUF, STR_NET_OK)) {
    ACK.ack_net_ok = true;
    goto EXIT;
  }

  if (strstr((char *)RECV_BUF, STR_MQTT_CONN_OK)) {
    ACK.ack_mqtt_connect_ok = true;
    goto EXIT;
  }

  if (strstr((char *)RECV_BUF, STR_MQTT_SUB_OK)) {
    ACK.ack_mqtt_sub_ok = true;
    goto EXIT;
  }

  if (strstr((char *)RECV_BUF, STR_MQTT_CLOSE_OK)) {
    ACK.ack_mqtt_close_ok = true;
    goto EXIT;
  }

  if (strstr((char *)RECV_BUF, STR_HTTP_ACTION)) {
    rs232_sim_air720h_recv_manage_process_action((char *)RECV_BUF);
    goto EXIT;
  }

  ESP_LOGW(TAG, "%4d %s unexcept recv %s", __LINE__, __func__, RECV_BUF);

EXIT:
  RECV_BUF[0] = '\0';
  RECV_SIZE = 0;
}

static void rs232_sim_air720h_recv_manage_push_byte(uint8_t data) {
  if (RECV_SIZE < RS232_SIM_AIR720H_MAX_BUF_SIZE) {
    RECV_BUF[RECV_SIZE] = data;
    RECV_BUF[RECV_SIZE + 1] = '\0';
    RECV_SIZE++;
  } else {
    ESP_LOGE(TAG, "%4d %s RECV_SIZE reach max:%d", __LINE__, __func__,
             RECV_SIZE);
    return;
  }

  // http read dispatch
  if (rs232_sim_air720h_recv_manage_get_http_read_state() == true) {
    rs232_sim_air720h_recv_manage_process_http_read(data);
    return;
  }

  if (RECV_SIZE == strlen(STR_HTTP_READ)) {
    if (strstr((char *)RECV_BUF, STR_HTTP_READ)) {
      rs232_sim_air720h_recv_manage_set_http_read_state(true);
    }
  }

  // mqtt sub dispatch
  if (rs232_sim_air720h_recv_manage_get_mqtt_sub_state() == true) {
    rs232_sim_air720h_recv_manage_process_mqtt_sub(data);
    return;
  }

  if (RECV_SIZE == strlen(STR_MQTT_SUB)) {
    if (strstr((char *)RECV_BUF, STR_MQTT_SUB)) {
      rs232_sim_air720h_recv_manage_set_mqtt_sub_state(true);
    }
  }

  // one cmd
  if (data == '\n') {
    RECV_BUF[RECV_SIZE - 1] = '\0';
    rs232_sim_air720h_recv_manage_parse_msg();
  }
}

static void rs232_sim_air720h_recv_manage_loop(rs232_dev_config_t *dev_config) {
  int len = 0;
  uint8_t data = 0;

  while (true) {
    len = uart_read_bytes(dev_config->uart_num, &data, 1,
                          20 / portTICK_PERIOD_MS);
    {
      if (len == 1) {
        // debug here
        // printf("%c", data);
        rs232_sim_air720h_recv_manage_push_byte(data);
      }
    }
  }
}

// global func ================================================================

esp_err_t rs232_sim_air720h_recv_manage_init(rs232_dev_config_t *dev_config) {
  esp_err_t err = ESP_OK;

  xTaskCreate((TaskFunction_t)rs232_sim_air720h_recv_manage_loop,
              "NYT_4G_AIR720H_TASK", 4 * 1024, dev_config, 8, NULL);

  return err;
}

void rs232_sim_air720h_recv_manage_reset_ack() {
  ACK.ack_ok = false;
  ACK.ack_error = false;
  ACK.ack_download = false;
  ACK.ack_connect_ok = false;
  ACK.ack_net_ok = false;
  ACK.ack_mqtt_connect_ok = false;
  ACK.ack_mqtt_sub_ok = false;
  ACK.ack_mqtt_close_ok = false;
}

void rs232_sim_air720h_recv_manage_reset_cache() {
  RECV_BUF[0] = '\0';
  RECV_SIZE = 0;
}

rs232_sim_air720h_ack rs232_sim_air720h_recv_manage_get_ack() { return ACK; }

esp_err_t rs232_sim_air720h_recv_manage_get_ok() {
  if (ACK.ack_ok == true) {
    return ESP_OK;
  } else {
    return ESP_ERR_INVALID_RESPONSE;
  }
}

esp_err_t rs232_sim_air720h_recv_manage_get_error() {
  if (ACK.ack_error == true) {
    return ESP_OK;
  } else {
    return ESP_ERR_INVALID_RESPONSE;
  }
}

esp_err_t rs232_sim_air720h_recv_manage_get_download() {
  if (ACK.ack_download == true) {
    return ESP_OK;
  } else {
    return ESP_ERR_INVALID_RESPONSE;
  }
}

// recv msg handle

esp_err_t rs232_sim_air720h_recv_manage_get_connect_ok() {
  if (ACK.ack_connect_ok == true) {
    return ESP_OK;
  } else {
    return ESP_ERR_INVALID_RESPONSE;
  }
}

esp_err_t rs232_sim_air720h_recv_manage_get_net_ok() {
  if (ACK.ack_net_ok == true) {
    return ESP_OK;
  } else {
    return ESP_ERR_INVALID_RESPONSE;
  }
}

esp_err_t rs232_sim_air720h_recv_manage_get_mqtt_connect_ok() {
  if (ACK.ack_mqtt_connect_ok == true) {
    return ESP_OK;
  } else {
    return ESP_ERR_INVALID_RESPONSE;
  }
}

esp_err_t rs232_sim_air720h_recv_manage_get_mqtt_sub_ok() {
  if (ACK.ack_mqtt_sub_ok == true) {
    return ESP_OK;
  } else {
    return ESP_ERR_INVALID_RESPONSE;
  }
}

esp_err_t rs232_sim_air720h_recv_manage_get_mqtt_close_ok() {
  if (ACK.ack_mqtt_close_ok == true) {
    return ESP_OK;
  } else {
    return ESP_ERR_INVALID_RESPONSE;
  }
}
