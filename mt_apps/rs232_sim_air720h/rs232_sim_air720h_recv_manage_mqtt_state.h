#ifndef _RS232_SIM_AIR720H_RECV_MANAGE_MQTT_STATE_H
#define _RS232_SIM_AIR720H_RECV_MANAGE_MQTT_STATE_H

#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "string.h"

// help func ==================================================================

void rs232_sim_air720h_recv_manage_mqtt_reset();

// func =======================================================================

int32_t rs232_sim_air720h_recv_manage_get_mqtt_state();

esp_err_t rs232_sim_air720h_recv_manage_process_mqtt_state(char *buf);

esp_err_t rs232_sim_air720h_recv_manage_get_mqtt_state_finish();

#endif
