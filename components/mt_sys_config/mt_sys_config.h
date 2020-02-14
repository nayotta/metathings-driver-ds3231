#ifndef _MT_SYS_CONFIG_H
#define _MT_SYS_CONFIG_H

#include "esp_err.h"
#include "esp_system.h"

typedef struct _mt_sys_config_host {
  char *host;
  bool use_ssl;
  int32_t http_port;
  char *mqtt_host;
  int32_t mqtt_port;
  char *module_cred_id;
  char *module_cred_key;
} mt_sys_config_host;

// get simple api =============================================================

esp_err_t mt_sys_config_get_int32_config(char *config_name, int32_t *value);

char *mt_sys_config_get_string_config(char *config_name);

// set simple api =============================================================

esp_err_t mt_sys_config_set_int32_config(char *config_name, int32_t value);

esp_err_t mt_sys_config_set_string_config(char *config_name, char *value);

// complex api ================================================================

esp_err_t mt_sys_config_set_host_config(mt_sys_config_host *host_config);

mt_sys_config_host *mt_sys_config_get_host_config();

#endif
