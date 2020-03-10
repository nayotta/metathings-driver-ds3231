#ifndef _RS232_SIM_AIR720H_RECV_MANAGE_H
#define _RS232_SIM_AIR720H_RECV_MANAGE_H

#include "esp_err.h"
#include "esp_system.h"

// define =====================================================================

typedef struct _rs232_sim_air720h_ack {
  bool ack_ok;
  bool ack_error;
  bool ack_download;
  bool ack_connect_ok;
  bool ack_net_ok;
  bool ack_mqtt_connect_ok;
  bool ack_mqtt_sub_ok;
  bool ack_mqtt_close_ok;
} rs232_sim_air720h_ack;


typedef struct _rs232_sim_air720h_mqtt_read_t {
  char *topic;
  int32_t read_size;
  char *read_buf;
} _rs232_sim_air720h_mqtt_read_t;

// func =======================================================================

void rs232_sim_air720h_recv_manage_reset_ack();

rs232_sim_air720h_ack rs232_sim_air720h_recv_manage_get_ack();

esp_err_t rs232_sim_air720h_recv_manage_init();

// recv ack handle

esp_err_t rs232_sim_air720h_recv_manage_get_ok();

esp_err_t rs232_sim_air720h_recv_manage_get_error();

esp_err_t rs232_sim_air720h_recv_manage_get_download();

// recv msg handle

esp_err_t rs232_sim_air720h_recv_manage_get_connect_ok();

esp_err_t rs232_sim_air720h_recv_manage_get_net_ok();

esp_err_t rs232_sim_air720h_recv_manage_get_mqtt_connect_ok();

esp_err_t rs232_sim_air720h_recv_manage_get_mqtt_sub_ok();

esp_err_t rs232_sim_air720h_recv_manage_get_mqtt_close_ok();

#endif
