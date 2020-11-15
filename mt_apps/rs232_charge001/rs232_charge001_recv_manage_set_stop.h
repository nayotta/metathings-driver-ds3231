#ifndef _RS232_CHARGE001_RECV_MANAGE_SET_STOP_H
#define _RS232_CHARGE001_RECV_MANAGE_SET_STOP_H

#include "esp_err.h"
#include "esp_log.h"
#include "string.h"

#include "rs232_charge001.h"

// help func ==================================================================

void rs232_charge001_recv_manage_set_stop_reset();

// func =======================================================================

esp_err_t rs232_charge001_recv_manage_set_stop_parse(uint8_t *buf,
                                                       int32_t size);

esp_err_t rs232_charge001_recv_manage_set_stop_finish();

esp_err_t rs232_charge001_recv_manage_set_stop_response(int32_t *port,
                                                          int32_t *time);

#endif
