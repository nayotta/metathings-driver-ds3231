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

#endif
