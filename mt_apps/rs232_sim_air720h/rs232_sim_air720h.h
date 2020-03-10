#ifndef _RS232_SIM_AIR720H_H
#define _RS232_SIM_AIR720H_H

#include "esp_err.h"
#include "mt_module_http.h"

// func =======================================================================

// http api

esp_err_t rs232_sim_air720h_http_init(mt_module_http_t *module_http);

esp_err_t
rs232_sim_air720h_http_issue_module_token(mt_module_http_t *module_http);

esp_err_t rs232_sim_air720h_http_show_module(mt_module_http_t *module_http);

esp_err_t rs232_sim_air720h_http_heartbeat(mt_module_http_t *module_http);

esp_err_t rs232_sim_air720h_http_task(mt_module_http_t *module_http);

// mqtt api

esp_err_t rs232_sim_air720h_mqtt_init();

esp_err_t rs232_sim_air720h_mqtt_task();

esp_err_t rs232_sim_air720h_mqtt_sub();

esp_err_t rs232_sim_air720h_mqtt_pub();

// ntp api

esp_err_t rs232_sim_air720h_ntp_init();

esp_err_t rs232_sim_air720h_ntp_task();

esp_err_t rs232_sim_air720h_ntp_get();

#endif
