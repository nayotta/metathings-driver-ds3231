#ifndef _RS232_CHARGE001_SENT_MANAGE_H
#define _RS232_CHARGE001_SENT_MANAGE_H

#include "esp_err.h"
#include "esp_log.h"
#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#include "freertos/semphr.h"

#include "rs232_dev.h"

// define =====================================================================

typedef esp_err_t (*msg_handle)();

// func =======================================================================

esp_err_t rs232_charge001_sent_manage_init(rs232_dev_config_t *dev_config);

esp_err_t rs232_charge001_sent_manage_sent_and_wait_finish(
    uint8_t *buf, int buf_size, int timeout, esp_err_t (*msg_handle)());

#endif
