#ifndef _RS232_SIM_AIR720H_SENT_MANAGE_H
#define _RS232_SIM_AIR720H_SENT_MANAGE_H

#include "esp_err.h"
#include "esp_log.h"
#include "string.h"

#include "rs232_dev.h"

// define =====================================================================

typedef esp_err_t (*msg_handle)();

typedef esp_err_t (*ack_handle)();

// func =======================================================================

esp_err_t rs232_sim_air720h_sent_manage_init(rs232_dev_config_t *dev_config);

esp_err_t rs232_sim_air720h_sent_manage_sent(uint8_t *buf, int buf_size);

esp_err_t rs232_sim_air720h_sent_manage_sent_and_wait_finish(
    uint8_t *buf, int buf_size, int timeout, esp_err_t (*msg_handle)(),
    esp_err_t (*ack_handle)());

#endif
