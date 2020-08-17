#include "mt_config_http_server.h"

#include "esp_log.h"
#include <sys/param.h>

#include "esp_http_server.h"

#include "mt_nvs_config.h"

// static define ==============================================================

static const char *TAG = "mt_config_http_server";

static esp_err_t set_config_handler(httpd_req_t *req);
static esp_err_t get_config_handler(httpd_req_t *req);

static const httpd_uri_t set_config = {.uri = "/set_config",
                                       .method = HTTP_POST,
                                       .handler = set_config_handler,
                                       .user_ctx = NULL};

static const httpd_uri_t get_config = {.uri = "/get_config",
                                       .method = HTTP_GET,
                                       .handler = get_config_handler,
                                       .user_ctx = NULL};

// static func ===============================================================

static esp_err_t set_config_handler(httpd_req_t *req) {
  ESP_LOGI(TAG, "%4d %s http set_config", __LINE__, __func__);
  esp_err_t err = ESP_OK;
  char buf[1000];
  int ret = 0;

  ret = httpd_req_recv(req, buf, req->content_len);
  if (ret < 0) {
    ESP_LOGE(TAG, "%4d %s httpd_req_recv failed:%d", __LINE__, __func__, ret);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  } else {
    ESP_LOGI(TAG, "%4d %s get post:%s", __LINE__, __func__, buf);
  }

  err = mt_nvs_config_set_json_config(buf);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_set_json_config failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // End response
  httpd_resp_send(req, NULL, 0);

EXIT:
  if (err != ESP_OK) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "request failed");
  }

  return ESP_OK;
}

static esp_err_t get_config_handler(httpd_req_t *req) {
  ESP_LOGI(TAG, "%4d %s http get_config", __LINE__, __func__);
  esp_err_t err = ESP_OK;
  char *buf = NULL;

  buf = mt_nvs_config_get_json_config();
  if (buf == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_get_json_config NULL", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // End response
  httpd_resp_send(req, buf, strlen(buf));

EXIT:
  if (buf != NULL)
    free(buf);
  if (err != ESP_OK) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "request failed");
  }
  return ESP_OK;
}

// global func ================================================================

httpd_handle_t mt_config_http_server_start_webserver() {
  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  // Start the httpd server
  ESP_LOGI(TAG, "%4d %s Starting server on port: '%d'", __LINE__, __func__,
           config.server_port);
  if (httpd_start(&server, &config) == ESP_OK) {
    // Set URI handlers
    ESP_LOGI(TAG, "%4d %s Registering URI handlers", __LINE__, __func__);
    httpd_register_uri_handler(server, &set_config);
    httpd_register_uri_handler(server, &get_config);
    return server;
  }

  ESP_LOGI(TAG, "Error starting server!");
  return NULL;
}

void mt_config_http_server_stop_webserver(httpd_handle_t server) {
  httpd_stop(server);
}
