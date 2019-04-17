#include "freertos/event_groups.h"

#ifndef CONFIG_ESP32_WIFI_STATIC_RX_BUFFER_NUM
#define CONFIG_ESP32_WIFI_STATIC_RX_BUFFER_NUM 10
#endif

#ifndef CONFIG_ESP32_WIFI_DYNAMIC_RX_BUFFER_NUM
#define CONFIG_ESP32_WIFI_DYNAMIC_RX_BUFFER_NUM 32
#endif

#ifndef CONFIG_ESP32_WIFI_TX_BUFFER_TYPE
#define CONFIG_ESP32_WIFI_TX_BUFFER_TYPE 1
#endif

#ifndef _MT_SMARTCONFIG_H
#define _MT_SMARTCONFIG_H

void mt_smartconfig_set_light_handle(mt_gpio_light_t *light_handle);

void mt_smartconfig_task(void);

#endif
