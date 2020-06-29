#ifndef _RS232_SIM_AIR720H_H
#define _RS232_SIM_AIR720H_H

#include "esp_err.h"
#include "esp_log.h"
#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "mt_module_flow.h"
#include "mt_module_http.h"

#include "rs232_dev.h"

// func =======================================================================

// serial api

esp_err_t rs232_sim_air720h_serial_init(int uart_num, int rx_pin, int tx_pin,
                                        int baud_rate);

// http api

esp_err_t rs232_sim_air720h_http_init(mt_module_http_t *module_http);

esp_err_t
rs232_sim_air720h_http_issue_module_token(mt_module_http_t *module_http);

esp_err_t rs232_sim_air720h_http_show_module(mt_module_http_t *module_http);

esp_err_t rs232_sim_air720h_http_heartbeat(mt_module_http_t *module_http);

push_frame_res_t *
rs232_sim_air720h_http_push_frame_to_flow(mt_module_http_t *module_http,
                                          mt_module_flow_t *flow_in);

esp_err_t rs232_sim_air720h_http_task(mt_module_http_t *module_http);

// mqtt api

esp_err_t rs232_sim_air720h_mqtt_init();

esp_err_t rs232_sim_air720h_mqtt_pub(char *topic, uint8_t *buf, int size);

esp_err_t rs232_sim_air720h_mqtt_state(int32_t *state);

esp_err_t rs232_sim_air720h_mqtt_task(char *module_id, char *device_id,
                                      uint64_t session_id,
                                      void (*handle)(char *topic, void *buf,
                                                     int size));

// ntp api

esp_err_t rs232_sim_air720h_ntp_init();

esp_err_t rs232_sim_air720h_ntp_task();

esp_err_t rs232_sim_air720h_ntp_get();

#endif
