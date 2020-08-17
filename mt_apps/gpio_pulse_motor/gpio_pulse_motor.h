#ifndef _gpio_pulse_motor_H
#define _gpio_pulse_motor_H

#include "esp_err.h"
#include "esp_log.h"

#include "mt_module_flow.h"

// define =====================================================================

typedef struct _gpio_pusle_motor_data_t {
  char *type;
  bool state;
  float time;
} gpio_pusle_motor_data_t;

// help func ==================================================================

gpio_pusle_motor_data_t *gpio_pulse_motor_new_data();

void gpio_pulse_motor_free_data(gpio_pusle_motor_data_t *data);

mt_module_flow_struct_group_t *gpio_pulse_motor_get_flow_data();

mt_module_flow_struct_group_t *
gpio_pulse_motor_get_notify_data(char *type, bool state, float left_time);

// func =======================================================================

esp_err_t gpio_pulse_motor_set_state(bool state);

esp_err_t gpio_pulse_motor_get_state(bool *state_out, float *left_time);

// time uint:s
esp_err_t gpio_pulse_motor_set_state_with_delay(bool state, float time);

esp_err_t gpio_pulse_motor_task(int pin_a, int pin_a_on_level, int pin_b,
                                int pin_b_on_level);

#endif
