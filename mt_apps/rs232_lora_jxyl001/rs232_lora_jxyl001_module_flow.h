#ifndef _RS232_LORA_JXYL001_MODULE_FLOW_H
#define _RS232_LORA_JXYL001_MODULE_FLOW_H

#include "mt_module_flow.h"

typedef struct _module_jxyl001_flow_t {
  mt_module_flow_t *module_flow;
  Rs232_Lora_Jxyl001_Devs *devs;
} module_jxyl001_flow_t;

void rs232_lora_jxyl001_module_flow_task(mt_module_flow_t *module_flow);

#endif
