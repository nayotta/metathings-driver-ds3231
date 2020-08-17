#ifndef _GPIO_PULSE_MOTOR_UTILS_H
#define _GPIO_PULSE_MOTOR_UTILS_H

#include "esp_err.h"
#include "esp_log.h"

#include "mt_proto_pulse_motor.pb-c.h"

// help func ==================================================================

esp_err_t
gpio_pulse_motor_utils_check_set_state_req(MtPulseMotor__SetStateReq *req);

esp_err_t gpio_pulse_motor_utils_check_set_state_with_delay_req(
    MtPulseMotor__SetStateWithDelayReq *req);

#endif
