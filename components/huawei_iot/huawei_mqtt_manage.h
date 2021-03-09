#ifndef _HUAWEI_MQTT_MANAGE_H
#define _HUAWEI_MQTT_MANAGE_H

#include "stdint.h"
#include "stdio.h"

// define =====================================================================

typedef void (*huawei_mqtt_manage_handle_t)(char *msg, int size);

typedef struct _huawei_mqtt_msg_t {
  char *topic;
  uint8_t *buf;
  int buf_size;
} huawei_mqtt_msg_t;

typedef struct _huawei_mqtt_manage_t {
  int handle_size;
  huawei_mqtt_manage_handle_t *handles;
  char **serviceID;
  char **methods;
} huawei_mqtt_manage_t;

// help func ==================================================================

huawei_mqtt_msg_t *huawei_mqtt_manage_new_msg();

void huawei_mqtt_manage_free_msg(huawei_mqtt_msg_t *msg);

// func =======================================================================

void huawei_mqtt_manage_add_handle(void (*handle)(char *msg, int size),
                                   char *serviceID, char *method);

void huawei_mqtt_manage_handle_process(char *topic, void *buf, int size);

#endif
