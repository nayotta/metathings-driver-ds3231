#ifndef _GPIO_PULSE_MOTOR_LIGHT_H
#define _GPIO_PULSE_MOTOR_LIGHT_H

#include "esp_err.h"
#include "esp_log.h"

// func =======================================================================

void gpio_pulse_motor_light_task(int pin, int pin_on_level);

#endif
