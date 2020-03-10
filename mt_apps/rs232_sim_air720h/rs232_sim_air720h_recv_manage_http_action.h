#ifndef _RS232_SIM_AIR720H_RECV_MANAGE_HTTP_ACTION_H
#define _RS232_SIM_AIR720H_RECV_MANAGE_HTTP_ACTION_H

#include "esp_err.h"
#include "esp_system.h"
#include "stdlib.h"

// define =====================================================================
typedef enum rs232_sim_air720h_recv_manage_http_action_method_t {
  rs232_sim_air720h_recv_manage_http_action_method_get = 0,
  rs232_sim_air720h_recv_manage_http_action_method_post = 1,
};

typedef struct _rs232_sim_air720h_recv_manage_http_action {
  bool state;
  int method;
  int code;
  int size;
} rs232_sim_air720h_recv_manage_http_action;

// func =======================================================================

void rs232_sim_air720h_recv_manage_init_http_action(
    rs232_sim_air720h_recv_manage_http_action action);

void rs232_sim_air720h_recv_manage_reset_http_action();

void rs232_sim_air720h_recv_manage_get_http_get_action_state(
    rs232_sim_air720h_recv_manage_http_action action);

esp_err_t rs232_sim_air720h_recv_manage_get_http_get_action_finish();

void rs232_sim_air720h_recv_manage_process_action(char *buf_in);

esp_err_t rs232_sim_air720h_recv_manage_http_action_check_post(int code);

#endif
