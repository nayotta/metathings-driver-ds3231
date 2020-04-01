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

void mt_nvs_config_free_host(mt_nvs_host_t *host);

void mt_nvs_config_free_flows(mt_nvs_flows_t *flows);

void mt_nvs_config_free_module(mt_nvs_module_t *module);

// func =======================================================================

mt_nvs_host_t *mt_nvs_config_get_host_config();

esp_err_t mt_nvs_config_set_host_config(mt_nvs_host_t *host);

esp_err_t mt_nvs_config_get_module_num(int *num_out);

mt_nvs_module_t *mt_nvs_config_get_module(int index_in);

mt_nvs_flows_t *mt_nvs_config_get_flow(int mod_index);

char *mt_nvs_config_get_flow_name(int module_index, int flow_index);

char *mt_nvs_config_get_net_type();

esp_err_t mt_nvs_config_get_flow_interval(int32_t index, int32_t *interval);

#endif
