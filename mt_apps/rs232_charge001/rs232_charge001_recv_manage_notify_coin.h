#ifndef _RS232_CHARGE001_RECV_MANAGE_NOTIFY_COIN_H
#define _RS232_CHARGE001_RECV_MANAGE_NOTIFY_COIN_H

#include "esp_err.h"
#include "esp_log.h"
#include "string.h"

// func =======================================================================

void rs232_charge001_recv_manage_notify_coin_push_flow(uint8_t *buf,
                                                       uint32_t size);

#endif
