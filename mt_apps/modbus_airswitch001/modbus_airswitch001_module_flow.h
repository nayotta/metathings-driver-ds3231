#ifndef _MODULE_FLOW_AIRSWITCH001_H
#define _MODULE_FLOW_AIRSWITCH001_H

#include "esp_err.h"
#include "stdint.h"

#include "mt_module_flow.h"
#include "mt_module_http.h"

void modbus_airswitch_flow_task(mt_module_flow_t *module_flow);

esp_err_t module_notify_process(mt_module_flow_struct_group_t *group);

#endif
