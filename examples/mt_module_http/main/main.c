#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "esp_event_loop.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "esp_wifi.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"

#include "mt_module_http.h"
#include "mt_nvs_storage.h"

static const char *TAG = "MT_MODULE_HTTP_EXAMPLE";

#define WIFI_SSID "Nayotta"
#define WIFI_PASS "pink_man"
#define HTTP_HOST "10.1.1.130"
#define HTTP_PORT 8080

char *TEST_CRED_ID = "test_cred_id";
int TEST_TIMESTAMP = 10000000;
int TEST_NONCE = 10000;
char *TEST_HMAC = "test_hmac";
char *TEST_TOKEN = "test_token";
char *TEST_MODULE_ID = "test_module_id";
char *TEST_MODULE_NAME = "test_module_name";
char *TEST_OBJ_ID = "test_obj_id";
char *TEST_OBJ_PREFIX = "test_obj_prefix";
char *TEST_OBJ_NAME = "test_obj_name";
char *TEST_OBJ_NEW_NAME = "test_obj_new_name";
char *TEST_OBJ_CONTENT = "test_obj_content";

static EventGroupHandle_t wifi_event_group;
const static int CONNECTED_BIT = BIT0;

static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
  switch (event->event_id)
  {
  case SYSTEM_EVENT_STA_START:
    esp_wifi_connect();
    break;
  case SYSTEM_EVENT_STA_GOT_IP:
    xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);

    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    esp_wifi_connect();
    xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
    break;
  default:
    break;
  }
  return ESP_OK;
}

static void wifi_init(void)
{
  tcpip_adapter_init();
  wifi_event_group = xEventGroupCreate();
  ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  wifi_config_t wifi_config = {
      .sta =
          {
              .ssid = WIFI_SSID,
              .password = WIFI_PASS,
          },
  };
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
  ESP_LOGI(TAG, "start the WIFI SSID:[%s]", WIFI_SSID);
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_LOGI(TAG, "Waiting for wifi");
  xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true,
                      portMAX_DELAY);
}

esp_err_t test_mt_module_http_actions_issue_module_token()
{
  esp_err_t err = ESP_OK;
  token_t *tkn = NULL;

  ESP_LOGI(TAG, "=============================== %d %s test", __LINE__,
           __func__);

  credential_t cred_in = {
      .id = TEST_CRED_ID,
  };

  tkn = mt_module_http_actions_issue_module_token(&cred_in, TEST_TIMESTAMP,
                                                  TEST_NONCE, TEST_HMAC);
  if (tkn == NULL)
  {
    ESP_LOGE(TAG, "%d %s failed code=%d", __LINE__, __func__, err);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

esp_err_t test_mt_module_http_actions_show_module()
{
  esp_err_t err = ESP_OK;
  module_t *mdl;

  ESP_LOGI(TAG, "=============================== %d %s test", __LINE__,
           __func__);

  mdl = mt_module_http_actions_show_module(TEST_TOKEN);
  if (mdl == NULL)
  {
    ESP_LOGE(TAG, "%d %s failed code=%d", __LINE__, __func__, err);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

esp_err_t test_mt_module_http_actions_heartbeat()
{
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "=============================== %d %s test", __LINE__,
           __func__);

  module_t mod_in = {
      .name = TEST_MODULE_NAME,
  };

  err = mt_module_http_actions_heartbeat(TEST_TOKEN, 12345678910, &mod_in);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%d %s failed code=%d", __LINE__, __func__, err);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

esp_err_t test_mt_module_http_actions_put_object()
{
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "=============================== %d %s test", __LINE__,
           __func__);

  object_t obj_in = {
      .device = NULL,
      .prefix = TEST_OBJ_PREFIX,
      .name = TEST_OBJ_NAME,
  };
  obj_in.device = malloc(sizeof(device_t));
  obj_in.device->id = TEST_OBJ_ID;

  err =
      mt_module_http_actions_put_object(TEST_TOKEN, &obj_in, TEST_OBJ_CONTENT);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%d %s failed code=%d", __LINE__, __func__, err);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

esp_err_t test_mt_module_http_actions_remove_object()
{
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "=============================== %d %s test", __LINE__,
           __func__);

  object_t obj_in = {
      .prefix = TEST_OBJ_PREFIX,
      .name = TEST_OBJ_NAME,
  };
  obj_in.device = malloc(sizeof(device_t));
  obj_in.device->id = TEST_OBJ_ID;

  err = mt_module_http_actions_remove_object(TEST_TOKEN, &obj_in);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%d %s failed code=%d", __LINE__, __func__, err);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

esp_err_t test_mt_module_http_actions_rename_object()
{
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "=============================== %d %s test", __LINE__,
           __func__);

  object_t src_in = {
      .prefix = TEST_OBJ_NAME,
      .name = TEST_OBJ_NAME,
  };
  src_in.device = malloc(sizeof(device_t));
  src_in.device->id = TEST_OBJ_ID;

  object_t des_in = {
      .prefix = TEST_OBJ_NAME,
      .name = TEST_OBJ_NEW_NAME,
  };
  des_in.device = malloc(sizeof(device_t));
  des_in.device->id = TEST_OBJ_ID;

  err = mt_module_http_actions_rename_object(TEST_TOKEN, &src_in, &des_in);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%d %s failed code=%d", __LINE__, __func__, err);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

esp_err_t test_mt_module_http_actions_get_object()
{
  esp_err_t err = ESP_OK;
  object_t *obj = NULL;

  ESP_LOGI(TAG, "=============================== %d %s test", __LINE__,
           __func__);

  object_t obj_in = {
      .prefix = TEST_OBJ_PREFIX,
      .name = TEST_OBJ_NAME,
  };
  obj_in.device = malloc(sizeof(device_t));
  obj_in.device->id = TEST_OBJ_ID;

  obj = mt_module_http_actions_get_object(TEST_TOKEN, &obj_in);
  if (obj == NULL)
  {
    ESP_LOGE(TAG, "%d %s failed code=%d", __LINE__, __func__, err);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

esp_err_t test_mt_module_http_actions_get_object_content()
{
  esp_err_t err = ESP_OK;
  char *content = NULL;

  ESP_LOGI(TAG, "=============================== %d %s test", __LINE__,
           __func__);

  object_t obj_in = {
      .prefix = TEST_OBJ_PREFIX,
      .name = TEST_OBJ_NAME,
  };
  obj_in.device = malloc(sizeof(device_t));
  obj_in.device->id = TEST_OBJ_ID;

  content = mt_module_http_actions_get_object_content(TEST_TOKEN, &obj_in);
  if (content == NULL)
  {
    ESP_LOGE(TAG, "%d %s failed code=%d", __LINE__, __func__, err);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

esp_err_t test_mt_module_http_actions_list_objects()
{
  esp_err_t err = ESP_OK;
  uint8_t *objs = NULL;
  int objs_num;

  ESP_LOGI(TAG, "=============================== %d %s test", __LINE__,
           __func__);

  object_t obj_in = {
      .prefix = TEST_OBJ_PREFIX,
      .name = TEST_OBJ_NAME,
  };
  obj_in.device = malloc(sizeof(device_t));
  obj_in.device->id = TEST_OBJ_ID;

  objs = mt_module_http_actions_list_objects(TEST_TOKEN, &obj_in, &objs_num);
  if (objs == NULL)
  {
    ESP_LOGE(TAG, "%d %s failed code=%d", __LINE__, __func__, err);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

void app_main()
{
  int ret = ESP_OK;

  ESP_LOGI(TAG, "test begin");

  ret = mt_nvs_init();
  if (ret == false)
  {
    ESP_LOGE(TAG, "%d mt_nvs_init failed", __LINE__);
    return;
  }

  wifi_init();

  test_mt_module_http_actions_issue_module_token();

  test_mt_module_http_actions_show_module();

  test_mt_module_http_actions_heartbeat();

  test_mt_module_http_actions_put_object();

  test_mt_module_http_actions_remove_object();

  test_mt_module_http_actions_rename_object();

  test_mt_module_http_actions_get_object();

  test_mt_module_http_actions_get_object_content();

  test_mt_module_http_actions_list_objects();

  ESP_LOGI(TAG, "test end");
}
