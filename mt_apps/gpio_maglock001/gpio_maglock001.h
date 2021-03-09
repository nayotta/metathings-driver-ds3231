#ifndef _GPIO_MAGLOCK001_H
#define _GPIO_MAGLOCK001_H

#include "esp_err.h"
#include "esp_system.h"

#include "cJSON.h"

// global func ================================================================

esp_err_t gpio_maglock001_get_port_num(int *num);

esp_err_t gpio_maglock001_get_state(int port_index, bool *state_out);

esp_err_t gpio_maglock001_get_has_changed(bool *change);

esp_err_t gpio_maglock001_task();

cJSON *gpio_maglock001_get_flow_data();

#endif
