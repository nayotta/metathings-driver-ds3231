#ifndef _RS232_SIM_AIR720H_RECV_MANAGE_HTTP_READ_H
#define _RS232_SIM_AIR720H_RECV_MANAGE_HTTP_READ_H

#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "stdlib.h"
#include "string.h"

// func =======================================================================

void rs232_sim_air720h_recv_manage_http_read_reset();

bool rs232_sim_air720h_recv_manage_get_http_read_state();

void rs232_sim_air720h_recv_manage_set_http_read_state(bool state_in);

esp_err_t rs232_sim_air720h_recv_manage_process_http_read(uint8_t byte);

esp_err_t rs232_sim_air720h_recv_manage_get_http_read_finish();

char *rs232_sim_air720h_recv_manage_get_http_read_res();

#endif
