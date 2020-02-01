#ifndef _MT_MQTT_MANAGE_H
#define _MT_MQTT_MANAGE_H

#include "esp_err.h"
#include "mqtt_client.h"

typedef struct _mt_mqtt_manage_client_t {
  esp_mqtt_client_handle_t *client;
  char *module_id;
  char *device_id;
  uint64_t session_id;
} mt_mqtt_manage_client_t;

typedef struct _mt_mqtt_manage_t {
  int mqtt_num;
  mt_mqtt_manage_client_t **clients;
} mt_mqtt_manage_t;

esp_err_t mt_mqtt_manage_add(mt_mqtt_manage_client_t *client_in);

char *mt_mqtt_manage_get_module_id(esp_mqtt_client_handle_t *client);

esp_err_t mt_mqtt_manage_set_module_id(esp_mqtt_client_handle_t *client_in,
                                       char *module_id_in);

char *mt_mqtt_manag_get_device_id(esp_mqtt_client_handle_t *client_in);

esp_err_t mt_mqtt_manage_set_device_id(esp_mqtt_client_handle_t *client_in,
                                       char *device_id_in);

esp_err_t mt_mqtt_manage_get_session_id(esp_mqtt_client_handle_t *client_in,
                                        uint64_t *session_id_out);

esp_err_t mt_mqtt_manage_set_session_id(esp_mqtt_client_handle_t *client_in,
                                        uint64_t session_id_in);

#endif
