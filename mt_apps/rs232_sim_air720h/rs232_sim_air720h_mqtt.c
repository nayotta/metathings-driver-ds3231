#include "rs232_sim_air720h_mqtt.h"

#include "rs232_sim_air720h_recv_manage.h"
#include "rs232_sim_air720h_sent_manage.h"

#include "mt_utils.h"

// static define ==============================================================

static const char *TAG = "rs232_sim_air720_mqtt";

static int MQTT_CLIENT_ID_SIZE = 23;
static int MQTT_CMD_TIMEOUT = 500;   // 500ms
static int MQTT_CMD_MAX_SIZE = 1000; // max cmd size 1000
static int MQTT_PUB_MAX_SIZE = 1360; // max cmd size 1360

// global func ================================================================

void rs232_sim_air720h_recv_manage_process_mqtt_sub(uint8_t data) {}

// set config
esp_err_t rs322_sim_air720h_mqtt_set_client_config(char *username,
                                                   char *password) {
  esp_err_t err = ESP_OK;
  char *cmd = malloc(MQTT_CMD_MAX_SIZE);
  char *client_id = NULL;

  // check arg
  if (username == NULL) {
    ESP_LOGE(TAG, "%4d %s username NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }
  if (password == NULL) {
    ESP_LOGE(TAG, "%4d %s password NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  client_id = mt_utils_get_random_client_id(MQTT_CLIENT_ID_SIZE);
  printf("debug client id:%s", client_id);

  sprintf(cmd, "AT+MCONFIG=\"%s\",\"%s\",\"%s\"\r", client_id, username,
          password);

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), MQTT_CMD_TIMEOUT, NULL,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  free(cmd);
  return err;
}

// set host
esp_err_t rs322_sim_air720h_mqtt_set_host(char *host, char *port) {
  esp_err_t err = ESP_OK;
  char *cmd = malloc(MQTT_CMD_MAX_SIZE);

  // check arg
  if (host == NULL) {
    ESP_LOGE(TAG, "%4d %s host NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }
  if (port == NULL) {
    ESP_LOGE(TAG, "%4d %s error port:%s", __LINE__, __func__, port);
    return ESP_ERR_INVALID_ARG;
  }

  sprintf(cmd, "AT+MIPSTART=\"%s\",\"%s\"\r", host, port);

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), MQTT_CMD_TIMEOUT,
      rs232_sim_air720h_recv_manage_get_connect_ok,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  free(cmd);
  return err;
}

// set connect
esp_err_t rs322_sim_air720h_mqtt_set_connect(int keepalive_interval) {
  esp_err_t err = ESP_OK;
  char *cmd = malloc(MQTT_CMD_MAX_SIZE);

  // check arg
  if (keepalive_interval <= 0) {
    ESP_LOGE(TAG, "%4d %s error keepalive_interval:%d", __LINE__, __func__,
             keepalive_interval);
    return ESP_ERR_INVALID_ARG;
  }

  sprintf(cmd, "AT+MCONNECT=1,%d\r", keepalive_interval);

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), MQTT_CMD_TIMEOUT,
      rs232_sim_air720h_recv_manage_get_mqtt_connect_ok,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  free(cmd);
  return err;
}

// set sub topic
esp_err_t rs322_sim_air720h_mqtt_set_sub_topic(char *topic) {
  esp_err_t err = ESP_OK;
  char *cmd = malloc(MQTT_CMD_MAX_SIZE);

  // check arg
  if (topic == NULL) {
    ESP_LOGE(TAG, "%4d %s topic NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  sprintf(cmd, "AT+MSUB=\"%s\",0\r", topic);

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), MQTT_CMD_TIMEOUT,
      rs232_sim_air720h_recv_manage_get_mqtt_sub_ok,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  free(cmd);
  return err;
}

// set pub msg
esp_err_t rs322_sim_air720h_mqtt_set_pub_msg(char *topic, char *msg) {
  esp_err_t err = ESP_OK;
  char *cmd = malloc(MQTT_CMD_MAX_SIZE + MQTT_PUB_MAX_SIZE);

  // check arg
  if (topic == NULL) {
    ESP_LOGE(TAG, "%4d %s topic NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }
  if (msg == NULL) {
    ESP_LOGE(TAG, "%4d %s msg NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }
  if (strlen(msg) > MQTT_PUB_MAX_SIZE) {
    ESP_LOGE(TAG, "%4d %s msg:%d must small than %d", __LINE__, __func__),
        strlen(msg), MQTT_PUB_MAX_SIZE);
    return ESP_ERR_INVALID_ARG;
  }

  sprintf(cmd, "AT+MPUB=\"%s\",0,0,\"%s\"\r", topic, msg);

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), MQTT_CMD_TIMEOUT, NULL,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  free(cmd);
  return err;
}

// set sent mode
esp_err_t
rs322_sim_air720h_mqtt_set_sent_mode(RS232_SIM_AIR720H_SENT_MODE_T mode) {
  esp_err_t err = ESP_OK;
  char *cmd = malloc(MQTT_CMD_MAX_SIZE);

  sprintf(cmd, "AT+MQTTMODE=%d\r", mode);

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), MQTT_CMD_TIMEOUT, NULL,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  free(cmd);
  return err;
}

// set recv mode
esp_err_t
rs322_sim_air720h_mqtt_set_recv_mode(RS232_SIM_AIR720H_RECV_MODE_T mode) {
  esp_err_t err = ESP_OK;
  char *cmd = malloc(MQTT_CMD_MAX_SIZE);

  sprintf(cmd, "AT+MQTTMSGSET=%d\r", mode);

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), MQTT_CMD_TIMEOUT, NULL,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  free(cmd);
  return err;
}

esp_err_t rs322_sim_air720h_mqtt_set_mqtt_close() {
  esp_err_t err = ESP_OK;
  char *cmd = "AT+MDISCONNECT\r";

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), MQTT_CMD_TIMEOUT, NULL,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  return err;
}

// set close
esp_err_t rs322_sim_air720h_mqtt_set_close() {
  esp_err_t err = ESP_OK;
  char *cmd = "AT+MIPCLOSE\r";

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), MQTT_CMD_TIMEOUT, NULL,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  return err;
}
