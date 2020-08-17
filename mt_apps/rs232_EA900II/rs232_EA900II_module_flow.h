#ifndef _RS232_EA900II_MODULE_FLOW_H
#define _RS232_EA900II_MODULE_FLOW_H

#include "esp_err.h"
#include "stdint.h"

#include "mt_module_flow.h"
#include "mt_module_http.h"

void rs232_EA900II_module_flow_task(mt_module_flow_t *module_flow);

#endif
