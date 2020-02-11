#ifndef _MODULE_FLOW_AIRSWITCH001_H
#define _MODULE_FLOW_AIRSWITCH001_H

#include "esp_err.h"
#include "stdint.h"

#include "mt_module_flow.h"
#include "mt_module_http.h"

void module_flow_task(mt_module_flow_t *module_flow, char *task_name);

#endif
