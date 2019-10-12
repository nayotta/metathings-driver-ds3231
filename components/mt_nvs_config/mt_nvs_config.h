#ifndef _MT_NVS_CONFIG_H
#define _MT_NVS_CONFIG_H

#include "esp_err.h"

typedef struct _mt_nvs_host_t {
  char *host;
  char *mqtt_port;
  int http_port;
} mt_nvs_host_t;

typedef struct _mt_nvs_flows_t {
  int flow_num;
  char **flows;
} mt_nvs_flows_t;

typedef struct _mt_nvs_module_t {
  int index;
  char *id;
  char *key;
  char *name;
  mt_nvs_flows_t *flows;
} mt_nvs_module_t;

esp_err_t mt_nvs_config_get_host_config(mt_nvs_host_t *host_out);

esp_err_t mt_nvs_config_get_module_num(int *num_out);

esp_err_t mt_nvs_config_get_module(int index_in,
                                   mt_nvs_module_t *module_out);

#endif
