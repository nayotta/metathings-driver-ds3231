#ifndef HUAWEI_MQTT_H
#define HUAWEI_MQTT_H

#include "stdint.h"
#include "stdio.h"

#include "esp_err.h"

// define =====================================================================

#define HUAWEI_PLATFORM_STRING "huawei"

// func =======================================================================

esp_err_t huawei_mqtt_pub_msg(char *subtopic, char *buf, int size);

esp_err_t huawei_mqtt_init(void (*handle)(char *topic, void *buf, int size));

#endif
