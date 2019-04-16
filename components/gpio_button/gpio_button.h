#include "stdio.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#define SHORT_PRESS_INTERVAL_DEFAULT 50
#define LONG_PRESS_INTERVAL_DEFAULT 5000
#define DOUBLE_PRESS_INTERVAL_DEFAULT 500

typedef struct
{
    int pin;
    int pin_on_level;
    int short_press_interval;
    int long_press_interval;
    int double_press_interval;
    void (*mt_gpio_btn_short_press_callback)();
    void (*mt_gpio_btn_long_press_callback)();
    void (*mt_gpio_btn_double_press_callback)();
} mt_gpio_btn_t;

bool mt_btn_task(mt_gpio_btn_t *btn_handle);
