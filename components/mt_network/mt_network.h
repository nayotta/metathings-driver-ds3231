#ifndef _MT_NETWORK_H
#define _MT_NETWORK_H

#include "esp_err.h"
#include "mt_ethernet.h"
#include "mt_smartconfig.h"

esp_err_t mt_network_task(int light_pin, int light_pin_on_level, int btn_pin,
                          int btn_pin_on_level);

#endif
