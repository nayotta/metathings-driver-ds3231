#ifndef _RS232_SIM_AIR720H_HTTP_H
#define _RS232_SIM_AIR720H_HTTP_H

#include "esp_err.h"

// func =======================================================================

// connect GRPS
esp_err_t rs232_sim_air720h_http_conn();

// set APN
esp_err_t rs232_sim_air720h_http_set_apn();

// set PDP
esp_err_t rs232_sim_air720h_http_set_pdp();

// get pdp
esp_err_t rs232_sim_air720h_http_get_pdp();

// set init
esp_err_t rs232_sim_air720h_http_set_init();

// set ssl
esp_err_t rs232_sim_air720h_http_set_ssl();

// set para host
esp_err_t rs232_sim_air720h_http_set_para_host(char *host);

// set para cid
esp_err_t rs232_sim_air720h_http_set_para_cid();

// set para url
esp_err_t rs232_sim_air720h_http_set_para_url(char *url);

// set para head
esp_err_t rs232_sim_air720h_http_set_para_head(char *head);

// set post data_size
esp_err_t rs232_sim_air720h_http_set_post_data_size(uint32_t size);

// set post data
esp_err_t rs232_sim_air720h_http_set_post_data(uint8_t *data, int size);

// set post
esp_err_t rs232_sim_air720h_http_set_post();

// get response
char* rs232_sim_air720h_http_get_response();

// set close
esp_err_t rs232_sim_air720h_http_set_close();

#endif
