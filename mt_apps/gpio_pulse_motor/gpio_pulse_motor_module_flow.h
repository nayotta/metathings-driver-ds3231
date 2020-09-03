#ifndef _GPIO_PULSE_MOTOR_MODULE_FLOW_H
#define _GPIO_PULSE_MOTOR_MODULE_FLOW_H

#include "esp_err.h"
#include "esp_log.h"

#include "mt_module_flow.h"

// func =======================================================================

void gpio_pulse_motor_module_flow_task(mt_module_flow_t *module_flow);

void gpio_pulse_motor_module_notify_state_task(char *type, bool state,
                                               float time);

#endif
