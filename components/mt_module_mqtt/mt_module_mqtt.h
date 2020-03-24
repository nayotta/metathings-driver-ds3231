#ifndef _MT_MODULE_MQTT_H
#define _MT_MODULE_MQTT_H

#include "stream_frame.pb-c.h"

// define =====================================================================

typedef void (*mt_module_mqtt_app_handle_t)(
    Ai__Metathings__Component__DownStreamFrame *msg, char *module_id);

typedef struct _mt_module_mqtt_msg_t {
  char *topic;
  uint8_t *buf;
  int buf_size;
} mt_module_mqtt_msg_t;

typedef struct _mt_module_mqtt_handle_t {
  int handle_size;
  mt_module_mqtt_app_handle_t *handles;
  char **methods;
} mt_module_mqtt_handle_t;

// func =======================================================================

void mt_module_mqtt_add_handle(
    void (*handle)(Ai__Metathings__Component__DownStreamFrame *msg,
                   char module_id[128]),
    char *method);

void mt_module_mqtt_handle(char *topic, void *buf, int size);

#endif
