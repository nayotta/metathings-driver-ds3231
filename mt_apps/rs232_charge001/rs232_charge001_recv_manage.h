#ifndef _RS232_CHARGE001_RECV_MANAGE_H
#define _RS232_CHARGE001_RECV_MANAGE_H

#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "string.h"

#include "rs232_dev.h"

// func =======================================================================

esp_err_t rs232_charge001_recv_manage_init(rs232_dev_config_t *dev_config);

#endif
