#ifndef _RS232_CHARGE001_RECV_MANAGE_NOTIFY_OVER_H
#define _RS232_CHARGE001_RECV_MANAGE_NOTIFY_OVER_H

#include "esp_err.h"
#include "esp_log.h"
#include "string.h"

// func =======================================================================

void rs232_charge001_recv_manage_notify_over_push_flow(uint8_t *buf,
                                                       uint32_t size);

#endif
