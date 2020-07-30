#ifndef _RS232_SIM_AIR720H_FLOW_H
#define _RS232_SIM_AIR720H_FLOW_H

#include "esp_err.h"
#include "esp_log.h"

#include "mt_module_flow.h"

// func =======================================================================

esp_err_t rs232_sim_air720h_flow_process_pong();

esp_err_t rs232_sim_air720h_flow_process_all(char *topic, uint8_t *buf,
                                             uint8_t size);

esp_err_t rs232_sim_air720h_flow_task(mt_module_flow_t *module_flow);

#endif
