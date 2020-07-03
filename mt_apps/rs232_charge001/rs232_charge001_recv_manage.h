#ifndef _RS232_CHARGE001_RECV_MANAGE_H
#define _RS232_CHARGE001_RECV_MANAGE_H

#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "string.h"

#include "rs232_dev.h"

// define =====================================================================

typedef struct _rs232_charge001_recv_buf_t {
  uint8_t *buf;
  uint32_t size;
} rs232_charge001_recv_buf_t;

// help func ==================================================================

rs232_charge001_recv_buf_t *
rs232_charge001_new_recv_buf(uint8_t *buf, uint32_t size, uint32_t *left_size);

void rs232_charge001_free_recv_buf(rs232_charge001_recv_buf_t *data);

// func =======================================================================

esp_err_t rs232_charge001_recv_manage_init(rs232_dev_config_t *dev_config);

#endif
