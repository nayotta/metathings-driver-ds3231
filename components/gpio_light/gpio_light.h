#include "stdio.h"
#include "stdbool.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

#define DEFAULT_GPIO_LIGHT_PIN 27
#define DEFAULT_GPIO_LIGHT_PIN_ON_LEVEL 0

typedef struct{
    int pin;
    int pin_on_level;
    int blink_interval; // ms
    bool blink_en;
}mt_gpio_light_t;

mt_gpio_light_t *mt_gpio_light_default();

bool mt_gpio_light_set_on(mt_gpio_light_t *light_handle);

bool mt_gpio_light_set_off(mt_gpio_light_t *light_handle);

bool mt_gpio_light_set_blink(mt_gpio_light_t *light_handle, int intr);

bool mt_gpio_light_task(mt_gpio_light_t *light_handle);


