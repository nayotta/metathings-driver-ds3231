#include "stdint.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "gpio_button.h"
#include "gpio_light.h"
#include "mt_ethernet.h"
#include "mt_memory_manage.h"
#include "mt_module_flow_manage.h"
#include "mt_module_http.h"
#include "mt_module_mqtt.h"
#include "mt_mqtt.h"
#include "mt_nvs_config.h"
#include "mt_nvs_storage.h"
#include "mt_smartconfig.h"

#include "gpio_pulse_motor.h"
#include "gpio_pulse_motor_button.h"
#include "gpio_pulse_motor_light.h"
#include "gpio_pulse_motor_module_flow.h"
#include "gpio_pulse_motor_module_mqtt.h"

// global define ==============================================================

static const char *TAG = "GPIO_PULSE_MOTOR_WIFI";

int LIGHT_PIN = 14;
int LIGHT_PIN_ON_LEVEL = 0;

int BTN_PIN = 35;
int BTN_PIN_ON_LEVEL = 0;

int PINA = 13;
int PINA_ON_LEVEL = 1;
int PINB = 15;
int PINB_ON_LEVEL = 1;

int PULSE_LIGHT_PIN = 12;
int PULSE_LIGHT_PIN_ON_LEVEL = 0;

// main func ==================================================================

void app_main() {
  mt_module_http_t *module_http = NULL;
  mt_module_flow_t *module_flow = NULL;

  ESP_LOGI(TAG, "%4d %s start", __LINE__, __func__);

  // nvs
  mt_nvs_init();

  mt_memory_manage_task(true);

  // gpio pulse motor task
  if (gpio_pulse_motor_task(PINA, PINA_ON_LEVEL, PINB, PINB_ON_LEVEL) !=
      ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_pulse_motor_task failed", __LINE__, __func__);
    return;
  }

  // network
  mt_smartconfig_task(LIGHT_PIN, LIGHT_PIN_ON_LEVEL, BTN_PIN, BTN_PIN_ON_LEVEL);

  // btn and light
  gpio_pulse_motor_button_task(BTN_PIN, BTN_PIN_ON_LEVEL);
  gpio_pulse_motor_light_task(PULSE_LIGHT_PIN, PULSE_LIGHT_PIN_ON_LEVEL);

  // metathings module http task
  module_http = mt_module_http_new(1);
  if (module_http == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_module_http_new failed", __LINE__, __func__);
    return;
  }
  mt_module_http_task(module_http);

  // wait module session build
  while (module_http->module->id == NULL || module_http->session_id == 0 ||
         module_http->module->deviceID == NULL) {
    vTaskDelay(2000 / portTICK_RATE_MS);
  }

  // metathings mqtt task
  mt_mqtt_init(1, module_http->module->id, module_http->session_id,
               module_http->module->deviceID, mt_module_mqtt_handle);

  gpio_pulse_motor_module_mqtt_init();

  // metathings flow
  module_flow = mt_module_flow_new(1, 1, module_http);
  if (module_flow == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_new failed", __LINE__, __func__);
    return;
  }
  module_flow->push_frame_interval = 120 * 1000; // 120s
  mt_module_flow_task(module_flow);

  gpio_pulse_motor_module_flow_task(module_flow);
}
