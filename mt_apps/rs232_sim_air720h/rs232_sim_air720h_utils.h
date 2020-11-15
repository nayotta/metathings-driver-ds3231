#ifndef _RS232_SIM_AIR720H_UTILS_H
#define _RS232_SIM_AIR720H_UTILS_H

#include "esp_err.h"
#include "esp_log.h"
#include "mt_module_http.h"
#include "string.h"

// func =======================================================================

char *rs232_sim_air720h_utils_get_head();

char *rs232_sim_air720h_utils_get_head_with_token(char *token);

char *rs232_sim_air720h_utils_get_head_with_token_and_session(char *token,
                                                              uint64_t session);

uint8_t *rs232_sim_air720h_utils_hex_to_byte(uint8_t *buf_in, int size_in,
                                             int *size_out);

char *rs232_sim_air720h_utils_byte_to_hex(uint8_t *buf_in, int size_in);

#endif
