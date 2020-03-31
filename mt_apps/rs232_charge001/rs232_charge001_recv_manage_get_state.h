#ifndef _RS232_CHARGE001_RECV_MANAGE_GET_STATE_H
#define _RS232_CHARGE001_RECV_MANAGE_GET_STATE_H

#include "esp_err.h"
#include "esp_log.h"
#include "string.h"

#include "rs232_charge001.h"

// help func ==================================================================

void rs232_charge001_recv_manage_get_state_reset();

// func =======================================================================

esp_err_t rs232_charge001_recv_manage_get_state_parse(uint8_t *buf,
                                                      int32_t size);

esp_err_t rs232_charge001_recv_manage_get_state_finish();

rs232_charge001_state2_t *rs232_charge001_recv_manage_get_state_response();

#endif
