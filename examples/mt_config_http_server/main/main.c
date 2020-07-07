#include "stdio.h"

#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gpio_button.h"
#include "gpio_light.h"
#include "mt_ethernet.h"
#include "mt_memory_manage.h"
#include "mt_module_flow_manage.h"
#include "mt_module_http.h"
#include "mt_module_mqtt.h"
#include "mt_mqtt_lan.h"
#include "mt_nvs_config.h"
#include "mt_nvs_storage.h"
#include "mt_smartconfig.h"

#include "mt_config_http_server.h"
#include "rs232_dev.h"

// global define ==============================================================

static const char *TAG = "example-mt-config-http-server";

int LIGHT_PIN = 14;
int LIGHT_PIN_ON_LEVEL = 0;

int BTN_PIN = 35;
int BTN_PIN_ON_LEVEL = 0;

int TX_PIN = 13;
int RX_PIN = 15;
int EN_PIN = 05;

#define SDA_GPIO 32
#define SCL_GPIO 33

// gloabal func ===============================================================

void app_main() {
  httpd_handle_t handle = NULL;

  ESP_LOGI(TAG, "test begin");

  // nvs
  mt_nvs_init();

  // memory manage
  mt_memory_manage_task(true);

  // network
  // mt_smartconfig_task(LIGHT_PIN, LIGHT_PIN_ON_LEVEL, BTN_PIN, BTN_PIN_ON_LEVEL);
  // mt_ethernet_task(-1, -1);
  mt_ethernet_static_task(-1, -1);

  // http server
  handle = mt_config_http_server_start_webserver();

  vTaskDelay(30 * 1000 / portTICK_PERIOD_MS);

  // mt_config_http_server_stop_webserver(handle);

  ESP_LOGI(TAG, "test end");
}
