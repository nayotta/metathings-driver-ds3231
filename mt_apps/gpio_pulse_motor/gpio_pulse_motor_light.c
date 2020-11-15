#include "gpio_pulse_motor_light.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gpio_light.h"
#include "gpio_pulse_motor.h"

// static define ==============================================================

static const char *TAG = "GPIO_PULSE_MOTOR_LIGHT";

static int GET_STATE_INTERVAL = 200; // 200 ms

// static func ================================================================

static void gpio_pulse_motor_light_loop(mt_gpio_light_t *light_handle) {
  esp_err_t err = ESP_OK;
  bool state = false;
  float time = 0;

  mt_gpio_light_set_off(light_handle);

  while (true) {
    err = gpio_pulse_motor_get_state(&state, &time);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s gpio_pulse_motor_get_state failed", __LINE__,
               __func__);
      goto ERROR;
    }

    if (state == true) {
      mt_gpio_light_set_on(light_handle);
    } else {
      mt_gpio_light_set_off(light_handle);
    }

  ERROR:
    vTaskDelay(GET_STATE_INTERVAL / portTICK_RATE_MS);
  }
}

// global func ================================================================

void gpio_pulse_motor_light_task(int pin, int pin_on_level) {
  bool ret = false;
  mt_gpio_light_t *light_handle = NULL;

  light_handle = mt_gpio_light_new(pin, pin_on_level);
  ret = mt_gpio_light_task(light_handle);
  if (ret == false) {
    ESP_LOGE(TAG, "%4d %s mt_gpio_light_task failed", __LINE__, __func__);
    return;
  }

  xTaskCreate((TaskFunction_t)gpio_pulse_motor_light_loop,
              "GPIO_PULSE_MOTOR_LIGHT_TASK", 4 * 1024, light_handle, 10, NULL);
}
