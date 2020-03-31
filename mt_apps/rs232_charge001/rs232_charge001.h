#ifndef _RS232_CHARGE001_H
#define _RS232_CHARGE001_H

#include "esp_err.h"
#include "esp_log.h"

#include "mt_proto_charge001.pb-c.h"
#include "rs232_charge001_recv_manage.h"
#include "rs232_charge001_sent_manage.h"
#include "rs232_dev.h"

// define =====================================================================

typedef struct _rs232_charge001_state1_t {
  int32_t port;
  int32_t state;
} rs232_charge001_state1_t;

typedef struct _rs232_charge001_state2_t {
  int32_t port;
  int32_t state;
  int32_t lefttime;
  int32_t power;
} rs232_charge001_state2_t;

typedef struct _rs232_charge001_states_t {
  int32_t num;
  rs232_charge001_state1_t **states;
} rs232_charge001_states_t;

// func =======================================================================

// other

esp_err_t rs232_charge001_init(int32_t uart_num, int32_t rx_pin,
                               int32_t tx_pin);

// api

rs232_charge001_state2_t *rs232_charge001_get_state(int32_t port);

rs232_charge001_states_t *rs232_charge001_get_states();

esp_err_t rs232_charge001_set_charge();

#endif
