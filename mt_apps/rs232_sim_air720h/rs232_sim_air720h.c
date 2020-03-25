#include "rs232_sim_air720h.h"

#include "mt_module_flow.h"
#include "mt_module_http.h"
#include "mt_module_http_utils.h"
#include "mt_mqtt_utils.h"
#include "mt_nvs_config.h"
#include "mt_utils.h"
#include "mt_utils_login.h"
#include "mt_utils_session.h"

#include "rs232_sim_air720h_http.h"
#include "rs232_sim_air720h_mqtt.h"
#include "rs232_sim_air720h_recv_manage.h"
#include "rs232_sim_air720h_recv_manage_http_action.h"
#include "rs232_sim_air720h_sent_manage.h"
#include "rs232_sim_air720h_utils.h"

// static define ==============================================================

static const char *TAG = "rs232_sim_air720";

// http
static char *ISSUE_TOKEN_PATH = "/v1/device_cloud/actions/issue_module_token";
static char *SHOW_MODULE_PATH = "/v1/device_cloud/actions/show_module";
static char *HEARTBEAT_PATH = "/v1/device_cloud/actions/heartbeat";
static char *PUSHFRAME_PATH = "/v1/device_cloud/actions/push_frame_to_flow";
#define AIR720H_URL_MAX_SIZE 100

// mqtt
void (*MSG_PROCESS)(char *topic, void *buf, int size);
static char *MODULE_ID = NULL;
static uint64_t SESSION_ID = 0;
static char *DEVICE_ID = NULL;

// static func ================================================================

static void rs232_sim_air720h_http_task_loop(mt_module_http_t *module_http) {
  int init_interval = 30 * 1000;               // 30s
  int issue_module_token_interval = 30 * 1000; // 30s
  int show_module_retry_max = 10;
  int show_module_retry_count = 10;
  int show_module_interval = 30 * 1000; // 30s
  int heartbeat_max = 6;
  int heartbeat_count = 6;
  int heartbeat_interval = 30 * 1000; // 30s
  esp_err_t err = ESP_OK;

RESTART:
  // http init
  while (true) {
    err = rs232_sim_air720h_http_init(module_http);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_init failed", __LINE__,
               __func__);
    } else {
      ESP_LOGI(TAG, "%4d %s rs232_sim_air720h_http_init success", __LINE__,
               __func__);
      break;
    }

    vTaskDelay(init_interval / portTICK_PERIOD_MS);
  }

  // issue token loop
  while (true) {
    err = rs232_sim_air720h_http_issue_module_token(module_http);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_issue_module_token failed",
               __LINE__, __func__);
    } else {
      ESP_LOGI(TAG, "%4d %s rs232_sim_air720h_http_issue_module_token success",
               __LINE__, __func__);
      break;
    }

    vTaskDelay(issue_module_token_interval / portTICK_PERIOD_MS);
  }

  show_module_retry_count = show_module_retry_max;

  // show module loop
  while (true) {
    if (module_http->module != NULL) {
      mt_module_http_utils_free_module(module_http->module);
      module_http->module = NULL;
    }

    if (show_module_retry_count <= 0) {
      ESP_LOGE(TAG, "%4d %s show_module_retry_count get limit, restart loop",
               __LINE__, __func__);
      goto RESTART;
    }

    err = rs232_sim_air720h_http_show_module(module_http);
    if (module_http->module == NULL) {
      ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_show_module failed",
               __LINE__, __func__);
      show_module_retry_count--;
    } else {
      ESP_LOGI(TAG, "%4d %s rs232_sim_air720h_http_show_module success",
               __LINE__, __func__);
      break;
    }
    vTaskDelay(show_module_interval / portTICK_PERIOD_MS);
  }

  // heartbeat loop
  heartbeat_count = heartbeat_max;
  module_http->session_id =
      mt_utils_session_new_session(mt_utils_session_gen_startup_session(),
                                   mt_utils_session_gen_major_session());

  while (true) {
    if (heartbeat_count <= 0) {
      ESP_LOGE(TAG, "%4d %s heartbeat_count get limit, restart loop", __LINE__,
               __func__);
      goto RESTART;
    }

    err = rs232_sim_air720h_http_heartbeat(module_http);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_heartbeat failed", __LINE__,
               __func__);
      heartbeat_count--;
    } else {
      ESP_LOGI(TAG, "%4d %s rs232_sim_air720h_http_heartbeat success", __LINE__,
               __func__);
      heartbeat_count = heartbeat_max;
    }

    vTaskDelay(heartbeat_interval / portTICK_PERIOD_MS);
  }

  return;
}

static void rs232_sim_air720h_mqtt_task_loop() {
  int mqtt_init_interval = 30 * 1000; // 30s
  esp_err_t err = ESP_OK;

  // RESTART:
  // mqtt_init loop
  while (true) {
    err = rs232_sim_air720h_mqtt_init();
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_mqtt_init failed", __LINE__,
               __func__);
    } else {
      ESP_LOGI(TAG, "%4d %s rs232_sim_air720h_mqtt_init success", __LINE__,
               __func__);
      break;
    }

    vTaskDelay(mqtt_init_interval / portTICK_PERIOD_MS);
  }

  // mqtt state check loop
  // (TODO ZH) check mqtt state and restart if disconnect
  while (true) {
    vTaskDelay(mqtt_init_interval / portTICK_PERIOD_MS);
  }
}

// global func ================================================================

esp_err_t rs232_sim_air720h_serial_init(int uart_num, int rx_pin, int tx_pin,
                                        int baud_rate) {
  esp_err_t err = ESP_OK;

  rs232_dev_config_t *dev_config = rs232_dev_default_new();

  dev_config->uart_num = uart_num;
  dev_config->rx_pin = rx_pin;
  dev_config->tx_pin = tx_pin;
  dev_config->uart_config->baud_rate = baud_rate;

  err = rs232_dev_init(dev_config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_dev_init failed", __LINE__, __func__);
    return err;
  }

  err = rs232_sim_air720h_sent_manage_init(dev_config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_sent_manage_init failed", __LINE__,
             __func__);
    return err;
  }

  err = rs232_sim_air720h_recv_manage_init(dev_config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_recv_manage_init failed", __LINE__,
             __func__);
    return err;
  }

  ESP_LOGI(TAG, "%4d %s init success", __LINE__, __func__);

  return ESP_OK;
}

esp_err_t rs232_sim_air720h_http_init(mt_module_http_t *module_http) {
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "%4d %s begin", __LINE__, __func__);

  if (module_http == NULL) {
    ESP_LOGE(TAG, "%4d %s module_http NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // connect
  err = rs232_sim_air720h_http_conn();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_conn failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // set apn
  err = rs232_sim_air720h_http_set_apn();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_apn failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // set pdp
  err = rs232_sim_air720h_http_set_pdp();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_pdp failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // set init
  err = rs232_sim_air720h_http_set_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_init failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // set ssl
  err = rs232_sim_air720h_http_set_ssl();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_ssl failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // set host
  err = rs232_sim_air720h_http_set_para_host(module_http->host);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_para_host failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // set para cid
  err = rs232_sim_air720h_http_set_para_cid();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_para_cid failed", __LINE__,
             __func__);
    goto EXIT;
  }

EXIT:
  if (err == ESP_OK)
    ESP_LOGI(TAG, "%4d %s http init success", __LINE__, __func__);
  else
    ESP_LOGE(TAG, "%4d %s ihttp init  failed", __LINE__, __func__);

  return err;
}

esp_err_t
rs232_sim_air720h_http_issue_module_token(mt_module_http_t *module_http) {
  esp_err_t err = ESP_OK;
  char url[AIR720H_URL_MAX_SIZE] = "";
  char *head = NULL;
  char *post_data = NULL;
  char *res_data = NULL;
  token_t *token_out = NULL;

  ESP_LOGI(TAG, "%4d %s begin", __LINE__, __func__);

  if (module_http == NULL) {
    ESP_LOGE(TAG, "%4d %s module_http NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // check arg
  if (module_http->host == NULL) {
    ESP_LOGE(TAG, "%4d %s host NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  sprintf(url, "http://%s%s", module_http->host, ISSUE_TOKEN_PATH);

  // set url
  err = rs232_sim_air720h_http_set_para_url(url);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_para_url failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // set head
  head = rs232_sim_air720h_utils_get_head();
  err = rs232_sim_air720h_http_set_para_head(head);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_para_head failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // set post data size
  post_data = mt_utils_login_get_issue_token_data(module_http);
  if (post_data == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_utils_login_get_issue_token_data failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  int post_size = strlen(post_data);
  err = rs232_sim_air720h_http_set_post_data_size(post_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_post_data_size failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // set post data
  err = rs232_sim_air720h_http_set_post_data((uint8_t *)post_data, post_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_post_data failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // set post
  err = rs232_sim_air720h_http_set_post();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_post failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // check res code
  err = rs232_sim_air720h_recv_manage_http_action_check_post(201);
  if (err != ESP_OK) {
    ESP_LOGE(
        TAG,
        "%4d %s rs232_sim_air720h_recv_manage_http_action_check_post failed",
        __LINE__, __func__);
    goto EXIT;
  }

  // get response
  res_data = rs232_sim_air720h_http_get_response();
  if (res_data == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_get_response failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // parse response
  token_out = mt_module_http_utils_parse_token_res(res_data);
  if (token_out == NULL) {
    ESP_LOGE(TAG, "%4d %s token_out NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  } else {
    if (token_out->text == NULL) {
      ESP_LOGE(TAG, "%4d %s token_out->text NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_RESPONSE;
      goto EXIT;
    }
  }

  if (module_http->token != NULL) {
    free(module_http->token);
  }

  module_http->token = malloc(strlen(token_out->text) + 1);
  module_http->token[strlen(token_out->text)] = '\0';
  memcpy(module_http->token, token_out->text, strlen(token_out->text));

EXIT:
  if (err == ESP_OK)
    ESP_LOGI(TAG, "%4d %s issue token success:%s", __LINE__, __func__,
             token_out->text);
  else
    ESP_LOGE(TAG, "%4d %s issue token failed", __LINE__, __func__);

  if (head != NULL)
    free(head);
  if (post_data != NULL)
    free(post_data);
  if (res_data != NULL)
    free(res_data);
  if (token_out != NULL)
    mt_module_http_utils_free_token(token_out);
  return err;
}

esp_err_t rs232_sim_air720h_http_show_module(mt_module_http_t *module_http) {
  esp_err_t err = ESP_OK;
  char url[AIR720H_URL_MAX_SIZE] = "";
  char *head = NULL;
  char *res_data = NULL;
  module_t *mdl_out = NULL;

  ESP_LOGI(TAG, "%4d %s begin", __LINE__, __func__);

  if (module_http == NULL) {
    ESP_LOGE(TAG, "%4d %s module_http NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // check arg
  if (module_http->host == NULL) {
    ESP_LOGE(TAG, "%4d %s host NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  if (module_http->token == NULL) {
    ESP_LOGE(TAG, "%4d %s token NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  sprintf(url, "http://%s%s", module_http->host, SHOW_MODULE_PATH);

  // set url
  err = rs232_sim_air720h_http_set_para_url(url);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_para_url failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // set head
  head = rs232_sim_air720h_utils_get_head_with_token(module_http->token);
  err = rs232_sim_air720h_http_set_para_head(head);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_para_head failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // set post
  err = rs232_sim_air720h_http_set_post();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_post failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // check res code
  err = rs232_sim_air720h_recv_manage_http_action_check_post(200);
  if (err != ESP_OK) {
    ESP_LOGE(
        TAG,
        "%4d %s rs232_sim_air720h_recv_manage_http_action_check_post failed",
        __LINE__, __func__);
    goto EXIT;
  }

  // get response
  res_data = rs232_sim_air720h_http_get_response();
  if (res_data == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_get_response failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // parse response
  mdl_out = mt_module_http_uitls_parse_module_res(res_data);
  if (mdl_out == NULL) {
    ESP_LOGE(TAG, "%4d %s mdl_out NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  } else {
    if (mdl_out->id == NULL) {
      ESP_LOGE(TAG, "%4d %s mdl_out->id NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_RESPONSE;
      goto EXIT;
    }
  }

  if (module_http->module != NULL) {
    mt_module_http_utils_free_module(module_http->module);
  }

  module_http->module = mdl_out;

EXIT:
  if (err == ESP_OK)
    ESP_LOGI(TAG, "%4d %s show module success", __LINE__, __func__);
  else
    ESP_LOGE(TAG, "%4d %s show module failed", __LINE__, __func__);

  if (head != NULL)
    free(head);
  if (res_data != NULL)
    free(res_data);
  return err;
}

esp_err_t rs232_sim_air720h_http_heartbeat(mt_module_http_t *module_http) {
  esp_err_t err = ESP_OK;
  char url[AIR720H_URL_MAX_SIZE] = "";
  char *head = NULL;
  char *post_data = NULL;

  ESP_LOGI(TAG, "%4d %s begin", __LINE__, __func__);

  if (module_http == NULL) {
    ESP_LOGE(TAG, "%4d %s module_http NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // check arg
  if (module_http->host == NULL) {
    ESP_LOGE(TAG, "%4d %s host NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  if (module_http->token == NULL) {
    ESP_LOGE(TAG, "%4d %s token NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  sprintf(url, "http://%s%s", module_http->host, HEARTBEAT_PATH);

  // set url
  err = rs232_sim_air720h_http_set_para_url(url);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_para_url failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // set head
  head = rs232_sim_air720h_utils_get_head_with_token_and_session(
      module_http->token, module_http->session_id);
  err = rs232_sim_air720h_http_set_para_head(head);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_para_head failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // set post data size
  post_data = mt_utils_login_get_heartbeat_data(module_http);
  if (post_data == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_utils_login_get_heartbeat_data failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  int post_size = strlen(post_data);
  err = rs232_sim_air720h_http_set_post_data_size(post_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_post_data_size failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // set post data
  err = rs232_sim_air720h_http_set_post_data((uint8_t *)post_data, post_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_post_data failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // set post
  err = rs232_sim_air720h_http_set_post();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_post failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // check res code
  err = rs232_sim_air720h_recv_manage_http_action_check_post(204);
  if (err != ESP_OK) {
    ESP_LOGE(
        TAG,
        "%4d %s rs232_sim_air720h_recv_manage_http_action_check_post failed",
        __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  if (err == ESP_OK)
    ESP_LOGI(TAG, "%4d %s heartbeat success", __LINE__, __func__);
  else
    ESP_LOGE(TAG, "%4d %s heartbeat failed", __LINE__, __func__);

  if (head != NULL)
    free(head);
  if (post_data != NULL)
    free(post_data);
  return err;
}

push_frame_res_t *
rs232_sim_air720h_http_push_frame_to_flow(mt_module_http_t *module_http,
                                          mt_module_flow_t *flow_in) {
  esp_err_t err = ESP_OK;
  char url[AIR720H_URL_MAX_SIZE] = "";
  char *head = NULL;
  char *res_data = NULL;
  char *post_data = NULL;
  push_frame_res_t *res_out = NULL;

  ESP_LOGI(TAG, "%4d %s begin", __LINE__, __func__);

  // check arg
  if (module_http == NULL) {
    ESP_LOGE(TAG, "%4d %s module_http NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  } else {
    if (module_http->host == NULL) {
      ESP_LOGE(TAG, "%4d %s host NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
    if (module_http->token == NULL) {
      ESP_LOGE(TAG, "%4d %s token NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  sprintf(url, "http://%s%s", module_http->host, PUSHFRAME_PATH);

  // set url
  err = rs232_sim_air720h_http_set_para_url(url);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_para_url failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // set head
  head = rs232_sim_air720h_utils_get_head_with_token(module_http->token);
  err = rs232_sim_air720h_http_set_para_head(head);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_para_head failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // set post data size
  post_data = mt_utils_login_get_push_frame_to_flow_data(module_http, flow_in);
  if (post_data == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_utils_login_get_push_frame_to_flow_data failed",
             __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  int post_size = strlen(post_data);
  err = rs232_sim_air720h_http_set_post_data_size(post_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_post_data_size failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // set post data
  err = rs232_sim_air720h_http_set_post_data((uint8_t *)post_data, post_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_post_data failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // set post
  err = rs232_sim_air720h_http_set_post();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_set_post failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // check res code
  err = rs232_sim_air720h_recv_manage_http_action_check_post(200);
  if (err != ESP_OK) {
    ESP_LOGE(
        TAG,
        "%4d %s rs232_sim_air720h_recv_manage_http_action_check_post failed",
        __LINE__, __func__);
    goto EXIT;
  }

  // get response
  res_data = rs232_sim_air720h_http_get_response();
  if (res_data == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_get_response failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // parse response
  res_out = mt_module_http_utils_parse_push_frame_res(res_data);
  if (res_out == NULL) {
    ESP_LOGE(TAG, "%4d %s res_out NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  } else {
    if (res_out->session_id == NULL) {
      ESP_LOGE(TAG, "%4d %s res_out->session_id NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_RESPONSE;
      goto EXIT;
    }
  }

EXIT:
  if (err == ESP_OK)
    ESP_LOGI(TAG, "%4d %s push_frame_to_flow success, id:%s", __LINE__,
             __func__, res_out->session_id);
  else
    ESP_LOGE(TAG, "%4d %s push_frame_to_flow failed", __LINE__, __func__);

  if (head != NULL)
    free(head);
  if (post_data != NULL)
    free(post_data);
  if (res_data != NULL)
    free(res_data);
  return res_out;
}

esp_err_t rs232_sim_air720h_http_task(mt_module_http_t *module_http) {
  // arg check
  if (module_http == NULL) {
    ESP_LOGE(TAG, "%4d %s module_http NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  } else {
    if (module_http->host == NULL) {
      ESP_LOGE(TAG, "%4d %s module_http->host NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }

    if (module_http->port == 0) {
      ESP_LOGE(TAG, "%4d %s module_http->port NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }

    if (module_http->cred == NULL) {
      ESP_LOGE(TAG, "%4d %s module_http->cred NULL", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    } else {
      if (module_http->cred->id == NULL) {
        ESP_LOGE(TAG, "%4d %s module_http->cred->id NULL", __LINE__, __func__);
        return ESP_ERR_INVALID_ARG;
      }

      if (module_http->cred->secret == NULL) {
        ESP_LOGE(TAG, "%4d %s module_http->cred->secret NULL", __LINE__,
                 __func__);
        return ESP_ERR_INVALID_ARG;
      }
    }

    if (module_http->event_handler == NULL) {
      ESP_LOGE(TAG, "%4d %s module_http->event_handle  NULL", __LINE__,
               __func__);
      return ESP_ERR_INVALID_ARG;
    }
  }

  xTaskCreate((TaskFunction_t)rs232_sim_air720h_http_task_loop,
              "rs232_sim_http_task", 8 * 1024, module_http, 10, NULL);
  return ESP_OK;
};

esp_err_t rs232_sim_air720h_mqtt_init() {
  esp_err_t err = ESP_OK;
  unsigned char *hmac_str = NULL;
  mt_nvs_host_t *host = NULL;
  mt_nvs_module_t *mod = NULL;
  char *module_topic = NULL;
  char *device_topic = NULL;

  ESP_LOGI(TAG, "%4d %s begin", __LINE__, __func__);

  // arg check
  if (DEVICE_ID == NULL) {
    ESP_LOGE(TAG, "%4d %s device_id NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  if (MODULE_ID == NULL) {
    ESP_LOGE(TAG, "%4d %s module_id NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // get config from nvs
  host = mt_nvs_config_get_host_config();
  if (host == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_get_host_config failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  mod = mt_nvs_config_get_module(1);
  if (mod == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_get_module index:%d failed", __LINE__,
             __func__, 1);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  hmac_str = mt_hmac_sha256_mqtt((uint8_t *)mod->key, strlen(mod->key),
                                 (uint8_t *)mod->id, strlen(mod->id));

  // clear mqtt connect
  err = rs322_sim_air720h_mqtt_set_mqtt_close();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs322_sim_air720h_mqtt_set_mqtt_close failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // clear connect
  err = rs322_sim_air720h_mqtt_set_close();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs322_sim_air720h_mqtt_set_close failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // config
  err = rs322_sim_air720h_mqtt_set_client_config(mod->id, (char *)hmac_str);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs322_sim_air720h_mqtt_set_client_config failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // set host
  err = rs322_sim_air720h_mqtt_set_host(host->host, host->mqtt_port);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs322_sim_air720h_mqtt_set_host failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // set connect
  err = rs322_sim_air720h_mqtt_set_connect(120);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs322_sim_air720h_mqtt_set_connect failed", __LINE__,
             __func__);
    goto EXIT;
  }

  // set sent mode
  err = rs322_sim_air720h_mqtt_set_sent_mode(RS232_SIM_AIR720H_SENT_MODE_HEX);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs322_sim_air720h_mqtt_set_sent_mode failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // set recv mode
  err =
      rs322_sim_air720h_mqtt_set_recv_mode(RS232_SIM_AIR720H_RECV_MODE_DIRECT);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs322_sim_air720h_mqtt_set_recv_mode failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // set module sub
  module_topic = mt_mqtt_utils_new_module_topic(MODULE_ID);
  if (module_topic == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_mqtt_utils_new_module_topic failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  err = rs322_sim_air720h_mqtt_set_sub_topic(module_topic);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs322_sim_air720h_mqtt_set_sub_topic failed",
             __LINE__, __func__);
    goto EXIT;
  }

  // set device sub
  device_topic = mt_mqtt_utils_new_device_topic(DEVICE_ID);
  if (device_topic == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_mqtt_utils_new_module_topic failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  err = rs322_sim_air720h_mqtt_set_sub_topic(device_topic);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs322_sim_air720h_mqtt_set_sub_topic failed",
             __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  mt_nvs_config_free_host(host);
  mt_nvs_config_free_module(mod);
  if (module_topic != NULL)
    free(module_topic);
  if (device_topic != NULL)
    free(device_topic);

  return err;
}

esp_err_t rs232_sim_air720h_mqtt_task(char *module_id, char *device_id,
                                      uint64_t session_id,
                                      void (*handle)(char *topic, void *buf,
                                                     int size)) {
  // arg check
  if (module_id == NULL) {
    ESP_LOGE(TAG, "%4d %s module_id NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }
  if (device_id == NULL) {
    ESP_LOGE(TAG, "%4d %s device_id NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }
  if (session_id == 0) {
    ESP_LOGE(TAG, "%4d %s session_id zero", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }
  if (handle == NULL) {
    ESP_LOGE(TAG, "%4d %s handle NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (MODULE_ID != NULL)
    free(MODULE_ID);
  MODULE_ID = module_id;

  if (DEVICE_ID != NULL)
    free(DEVICE_ID);
  DEVICE_ID = device_id;

  SESSION_ID = session_id;

  if (MSG_PROCESS != NULL)
    free(MSG_PROCESS);
  MSG_PROCESS = handle;

  xTaskCreate((TaskFunction_t)rs232_sim_air720h_mqtt_task_loop,
              "rs232_sim_http_task", 8 * 1024, NULL, 10, NULL);
  return ESP_OK;
}

esp_err_t rs232_sim_air720h_mqtt_pub(char *topic, uint8_t *buf, int size) {
  esp_err_t err = ESP_OK;
  char *hex_str = NULL;

  hex_str = rs232_sim_air720h_utils_byte_to_hex(buf, size);
  if (hex_str == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_utils_byte_to_hex NULL", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  err = rs322_sim_air720h_mqtt_set_pub_msg(topic, hex_str);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs322_sim_air720h_mqtt_set_pub_msg failed", __LINE__,
             __func__);
    goto EXIT;
  }

EXIT:
  if (hex_str != NULL)
    free(hex_str);
  return err;
}
