#ifndef _MT_SYS_MONITOR_H
#define _MT_SYS_MONITOR_H

#include "esp_err.h"

typedef struct _mt_sys_monitor_state {
  int32_t startup;
  int32_t restart_count;
  float free;
  int32_t error_count;
} mt_sys_monitor_state;

// set simple api =============================================================

esp_err_t mt_sys_monitor_set_restart_count();

esp_err_t mt_sys_monitor_set_error_count();

esp_err_t mt_sys_monitor_set_restart();

// get simple api =============================================================

esp_err_t mt_sys_monitor_get_startup(int32_t *startup_out);

esp_err_t mt_sys_monitor_get_restart_count(int32_t *restart_count_out);

esp_err_t mt_sys_monitor_get_free(float *free_out);

esp_err_t mt_sys_monitor_get_error_count(int32_t *error_count_out);

// complex api ================================================================

mt_sys_monitor_state *mt_sys_monitor_get_state();

#endif
