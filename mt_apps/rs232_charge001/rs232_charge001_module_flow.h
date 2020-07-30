#ifndef _RS232_CHARGE001_MODULE_FLOW_H
#define _RS232_CHARGE001_MODULE_FLOW_H

#include "mt_module_flow.h"

// func =======================================================================

void rs232_charge001_module_flow_task(mt_module_flow_t *module_flow);

void rs232_charge001_module_notify_state_task(int32_t port);

#endif
