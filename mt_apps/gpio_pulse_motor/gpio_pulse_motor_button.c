#include "gpio_pulse_motor_button.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gpio_button.h"
#include "gpio_pulse_motor.h"
#include "gpio_pulse_motor_module_flow.h"

// static define ==============================================================

static const char *TAG = "GPIO_PULSE_MOTOR_BUTTON";

// static func ================================================================

static void gpio_pulse_motor_button_short_press() {
  esp_err_t err = ESP_OK;
  bool state = false;
  bool new_state = false;
  float time = 0;

  err = gpio_pulse_motor_get_state(&state, &time);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_pulse_motor_get_state failed", __LINE__,
             __func__);
    return;
  }

  gpio_pulse_motor_clear_delay_task();

  if (state == true) {
    new_state = false;
  } else {
    new_state = true;
  }

  err = gpio_pulse_motor_set_state(new_state);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_pulse_motor_set_state failed", __LINE__,
             __func__);
    return;
  }
  gpio_pulse_motor_module_notify_state_task("local", new_state, 0);
}

// global func ================================================================

void gpio_pulse_motor_button_task(int btn_pin, int btn_pin_on_level) {
  mt_gpio_btn_t *gpio_btn_handle = NULL;

  if (btn_pin == -1)
    return;

  gpio_btn_handle = gpio_btn_get_handle(btn_pin);
  if (gpio_btn_handle == NULL) {
    gpio_btn_handle = mt_gpio_btn_default();
    gpio_btn_handle->pin = btn_pin;
    gpio_btn_handle->pin_on_level = btn_pin_on_level;
    gpio_btn_handle->mt_gpio_btn_short_press_callback =
        gpio_pulse_motor_button_short_press;

    if (mt_gpio_btn_task(gpio_btn_handle) == false) {
      ESP_LOGE(TAG, "%d %s mt_gpio_btn_task failed", __LINE__, __func__);
      return;
    }
  } else {
    gpio_btn_handle->mt_gpio_btn_short_press_callback =
        gpio_pulse_motor_button_short_press;
  }
}
