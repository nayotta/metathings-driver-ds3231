#ifndef _RS232_CHARGE001_LORA_FLOW_H
#define _RS232_CHARGE001_LORA_FLOW_H

#include "esp_err.h"
#include "esp_log.h"
#include "string.h"

#include "rs232_charge001.h"

// define =====================================================================

// help func ==================================================================

esp_err_t rs232_charge001_update_interval(int interval);

// func =======================================================================

esp_err_t rs232_charge001_lora_flow_task();

#endif
