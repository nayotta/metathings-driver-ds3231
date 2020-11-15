#ifndef _RS232_SIM_AIR720H_RECV_MANAGE_MQTT_SUB_H
#define _RS232_SIM_AIR720H_RECV_MANAGE_MQTT_SUB_H

#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "string.h"

// define =====================================================================

typedef struct _rs232_sim_air720h_mqtt_msg_t {
  char *topic;
  uint8_t *buf;
  uint32_t size;
} rs232_sim_air720h_mqtt_msg_t;

// help func ==================================================================

rs232_sim_air720h_mqtt_msg_t *
rs232_sim_air720h_new_mqtt_msg(char *topic, uint8_t *buf, uint32_t size);

void rs232_sim_air720h_free_mqtt_msg(rs232_sim_air720h_mqtt_msg_t *data);

// func =======================================================================

bool rs232_sim_air720h_recv_manage_get_mqtt_sub_state();

void rs232_sim_air720h_recv_manage_set_mqtt_sub_state(bool state_in);

esp_err_t rs232_sim_air720h_recv_manage_process_mqtt_sub(uint8_t byte);

#endif
