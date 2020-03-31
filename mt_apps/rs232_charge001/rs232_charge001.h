#ifndef _RS232_CHARGE001_H
#define _RS232_CHARGE001_H

#include "esp_err.h"
#include "esp_log.h"

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

// help func ==================================================================

rs232_charge001_state1_t *rs232_charge001_new_state1();

void rs232_charge001_free_state1(rs232_charge001_state1_t *state);

rs232_charge001_state2_t *rs232_charge001_new_state2();

void rs232_charge001_free_state2(rs232_charge001_state2_t *state);

rs232_charge001_states_t *rs232_charge001_new_states(int32_t num);

void rs232_charge001_free_states(rs232_charge001_states_t *states);

// func =======================================================================

esp_err_t rs232_charge001_init(int32_t uart_num, int32_t rx_pin,
                               int32_t tx_pin);

rs232_charge001_state2_t *rs232_charge001_get_state(int32_t port);

rs232_charge001_states_t *rs232_charge001_get_states();

esp_err_t rs232_charge001_set_charge(int32_t port, int32_t money, int32_t time,
                                     int32_t *res_port, int32_t *res_result);

#endif
