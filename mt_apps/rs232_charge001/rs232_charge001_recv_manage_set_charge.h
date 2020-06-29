#ifndef _RS232_CHARGE001_RECV_MANAGE_SET_CHARGE_H
#define _RS232_CHARGE001_RECV_MANAGE_SET_CHARGE_H

#include "esp_err.h"
#include "esp_log.h"
#include "string.h"

#include "rs232_charge001.h"

// help func ==================================================================

void rs232_charge001_recv_manage_set_charge_reset();

// func =======================================================================

esp_err_t rs232_charge001_recv_manage_set_charge_parse(uint8_t *buf,
                                                       int32_t size);

esp_err_t rs232_charge001_recv_manage_set_charge_finish();

esp_err_t rs232_charge001_recv_manage_set_charge_response(int32_t *port,
                                                          int32_t *result);

#endif
