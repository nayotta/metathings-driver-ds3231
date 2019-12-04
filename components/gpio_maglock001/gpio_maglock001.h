#ifndef _GPIO_MAGLOCK001_H
#define _GPIO_MAGLOCK001_H

#include "driver/gpio.h"
#include "stdio.h"

#include "esp_log.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

esp_err_t gpio_maglock001_get_port_num(int module_index, int *num_out);

esp_err_t gpio_maglock001_get_state(int module_index, int port_index,
                                    bool *state_out);

esp_err_t gpio_maglock001_get_has_changed(int module_index, bool *change);

esp_err_t gpio_maglock001_task(int module_index);

#endif
