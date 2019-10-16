#ifndef _MT_SMARTCONFIG_H
#define _MT_SMARTCONFIG_H

#include "freertos/event_groups.h"
#include "gpio_light.h"

#ifndef CONFIG_ESP32_WIFI_STATIC_RX_BUFFER_NUM
#define CONFIG_ESP32_WIFI_STATIC_RX_BUFFER_NUM 10
#endif

#ifndef CONFIG_ESP32_WIFI_DYNAMIC_RX_BUFFER_NUM
#define CONFIG_ESP32_WIFI_DYNAMIC_RX_BUFFER_NUM 32
#endif

#ifndef CONFIG_ESP32_WIFI_TX_BUFFER_TYPE
#define CONFIG_ESP32_WIFI_TX_BUFFER_TYPE 1
#endif

void mt_smartconfig_task(int light_pin, int light_pin_on_level, int btn_pin,
                         int btn_pin_on_level);

#endif
