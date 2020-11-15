#ifndef _GPIO_BUTTON_H
#define _GPIO_BUTTON_H

#include "stdbool.h"
#include "stdio.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

// define =====================================================================

#define DEFAULT_GPIO_BTN_PIN 14
#define DEFAULT_GPIO_BTN_PIN_ON_LEVEL 1
#define DEFAULT_SHORT_PRESS_INTERVAL 50
#define DEFAULT_LONG_PRESS_INTERVAL 5000
#define DEFAULT_DOUBLE_PRESS_INTERVAL 500

typedef struct {
  int pin;
  int pin_on_level;
  int short_press_interval;
  int long_press_interval;
  int double_press_interval;
  void (*mt_gpio_btn_short_press_callback)();
  void (*mt_gpio_btn_long_press_callback)();
  void (*mt_gpio_btn_double_press_callback)();
} mt_gpio_btn_t;

// func =======================================================================

mt_gpio_btn_t *mt_gpio_btn_default();

bool mt_gpio_btn_task(mt_gpio_btn_t *btn_handle);

mt_gpio_btn_t *gpio_btn_get_handle(int pin);

#endif
