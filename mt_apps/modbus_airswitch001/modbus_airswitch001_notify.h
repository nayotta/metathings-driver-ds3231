#ifndef _MODBUS_ARISWITCH001_NIOTIFY_H
#define _MODBUS_ARISWITCH001_NIOTIFY_H

#include "esp_err.h"
#include "esp_system.h"

// func =======================================================================

void modbus_airswitch001_notify_task();

esp_err_t modbus_airswitch001_notify_update_switch(int addr, bool state);

#endif
