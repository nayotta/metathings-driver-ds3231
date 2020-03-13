#include "rs232_sim_air720h_http.h"

#include "rs232_sim_air720h_recv_manage.h"
#include "rs232_sim_air720h_recv_manage_http_action.h"
#include "rs232_sim_air720h_recv_manage_http_read.h"
#include "rs232_sim_air720h_sent_manage.h"

// static define ==============================================================

static const char *TAG = "rs232_sim_air720_http";

static int HTTP_CMD_TIMEOUT = 500;           // 500ms
static int HTTP_CMD_MAX_SIZE = 200;          // max cmd size 200
static int HTTP_CMD_DOWNLOAD_TIMEOUT = 2000; // download timeout 2s
static int HTTP_POST_TIMEOUT = 5000;         // 5s

// static func ================================================================

// global func ================================================================

esp_err_t rs232_sim_air720h_http_conn() {
  esp_err_t err = ESP_OK;
  char *cmd = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r";

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), HTTP_CMD_TIMEOUT, NULL,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  return err;
}

esp_err_t rs232_sim_air720h_http_set_apn() {
  esp_err_t err = ESP_OK;
  char *cmd = "AT+SAPBR=3,1,\"APN\",\"\"\r";

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), HTTP_CMD_TIMEOUT, NULL,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  return err;
}

esp_err_t rs232_sim_air720h_http_set_pdp() {
  esp_err_t err = ESP_OK;
  char *cmd = "AT+SAPBR=1,1\r";

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), HTTP_CMD_TIMEOUT, NULL, NULL);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  return err;
}

esp_err_t rs232_sim_air720h_http_get_pdp() {
  // TODO(zh) unfinish
  return ESP_ERR_INVALID_ARG;
  /*
  esp_err_t err = ESP_OK;
  char *cmd = "AT+SAPBR=2,1\r";

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), HTTP_CMD_TIMEOUT, NULL,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  return err;*/
}

esp_err_t rs232_sim_air720h_http_set_init() {
  esp_err_t err = ESP_OK;
  char *cmd = "AT+HTTPINIT\r";

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), HTTP_CMD_TIMEOUT, NULL, NULL);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  return err;
}

esp_err_t rs232_sim_air720h_http_set_ssl() {
  esp_err_t err = ESP_OK;
  char *cmd = "AT+HTTPSSL=1\r";

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), HTTP_CMD_TIMEOUT, NULL,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  return err;
}

esp_err_t rs232_sim_air720h_http_set_para_host(char *host) {
  esp_err_t err = ESP_OK;
  char *cmd = malloc(HTTP_CMD_MAX_SIZE);

  // check arg
  if (host == NULL) {
    ESP_LOGE(TAG, "%4d %s host NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  sprintf(cmd, "AT+SSLCFG=\"hostname\",153,\"%s\"\r", host);

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), HTTP_CMD_TIMEOUT, NULL,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  free(cmd);
  return err;
}

esp_err_t rs232_sim_air720h_http_set_para_cid() {
  esp_err_t err = ESP_OK;
  char *cmd = "AT+HTTPPARA=\"CID\",1\r";

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), HTTP_CMD_TIMEOUT, NULL,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  return err;
}

esp_err_t rs232_sim_air720h_http_set_para_url(char *url) {
  esp_err_t err = ESP_OK;
  char *cmd = malloc(HTTP_CMD_MAX_SIZE);

  // check arg
  if (url == NULL) {
    ESP_LOGE(TAG, "%4d %s url NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  sprintf(cmd, "AT+HTTPPARA=\"URL\",\"%s\"\r", url);

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), HTTP_CMD_TIMEOUT, NULL,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  free(cmd);
  return err;
}

esp_err_t rs232_sim_air720h_http_set_para_head(char *head) {
  esp_err_t err = ESP_OK;
  char *cmd = malloc(HTTP_CMD_MAX_SIZE);

  // check arg
  if (head == NULL) {
    ESP_LOGE(TAG, "%4d %s head NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  sprintf(cmd, "AT+HTTPPARA=\"USERDATA\",\"%s\"\r", head);

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), HTTP_CMD_TIMEOUT, NULL,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  free(cmd);
  return err;
}

esp_err_t rs232_sim_air720h_http_set_post_data_size(uint32_t size) {
  esp_err_t err = ESP_OK;
  char *cmd = malloc(HTTP_CMD_MAX_SIZE);

  // check arg
  if (size == 0) {
    ESP_LOGE(TAG, "%4d %s size 0", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  sprintf(cmd, "AT+HTTPDATA=%d,%d\r", size, HTTP_CMD_DOWNLOAD_TIMEOUT);

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), HTTP_CMD_TIMEOUT, NULL,
      rs232_sim_air720h_recv_manage_get_download);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  free(cmd);
  return err;
}

esp_err_t rs232_sim_air720h_http_set_post_data(uint8_t *data, int size) {
  esp_err_t err = ESP_OK;

  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)data, size, HTTP_CMD_TIMEOUT, NULL,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  return err;
}

esp_err_t rs232_sim_air720h_http_set_post() {
  esp_err_t err = ESP_OK;
  char *cmd = "AT+HTTPACTION=1\r";

  // reset cache
  rs232_sim_air720h_recv_manage_reset_http_action();

  // reqeust
  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), HTTP_POST_TIMEOUT,
      rs232_sim_air720h_recv_manage_get_http_get_action_finish,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  return err;
}

char *rs232_sim_air720h_http_get_response() {
  esp_err_t err = ESP_OK;
  char *res_out = NULL;
  char *cmd = "AT+HTTPREAD\r";

  // reset cache
  rs232_sim_air720h_recv_manage_http_read_reset();

  // reqeust
  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), HTTP_CMD_TIMEOUT,
      rs232_sim_air720h_recv_manage_get_http_read_finish,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

  // response
  res_out = rs232_sim_air720h_recv_manage_get_http_read_res();

EXIT:
  return res_out;
}

esp_err_t rs232_sim_air720h_http_set_close() {
  esp_err_t err = ESP_OK;
  char *cmd = "AT+HTTPTERM\r";

  // reqeust
  err = rs232_sim_air720h_sent_manage_sent_and_wait_finish(
      (uint8_t *)cmd, strlen(cmd), HTTP_CMD_TIMEOUT, NULL,
      rs232_sim_air720h_recv_manage_get_ok);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s CMD failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  return err;
}
