#ifndef _MT_NVS_CONFIG_H
#define _MT_NVS_CONFIG_H

#include "esp_err.h"
#include "esp_system.h"

// define =====================================================================

typedef struct _mt_nvs_host_t {
  char *host;
  char *mqtt_port;
  int http_port;
  bool use_ssl;
  char *net_type;
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

// help func =================================================================

mt_nvs_host_t *mt_nvs_config_new_host();

mt_nvs_flows_t *mt_nvs_config_new_flows();

mt_nvs_module_t *mt_nvs_config_new_module();

void mt_nvs_config_free_flows(mt_nvs_flows_t *flows);

void mt_nvs_config_free_module(mt_nvs_module_t *module);

// func =======================================================================

esp_err_t mt_nvs_config_get_host_config(mt_nvs_host_t *host_out);

esp_err_t mt_nvs_config_set_host_config(mt_nvs_host_t *host);

esp_err_t mt_nvs_config_get_module_num(int *num_out);

esp_err_t mt_nvs_config_get_module(int index_in, mt_nvs_module_t *module_out);

esp_err_t mt_nvs_config_get_flow(int mod_index, mt_nvs_flows_t *flows);

char *mt_nvs_config_get_flow_name(int module_index, int flow_index);

char *mt_nvs_config_get_net_type();

#endif
