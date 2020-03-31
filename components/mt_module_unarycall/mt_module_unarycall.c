#include "string.h"

#include "esp_log.h"

#include "mt_module_unarycall.h"
#include "mt_module_unarycall_config.h"
#include "mt_module_unarycall_monitor.h"
#include "mt_module_unarycall_ota.h"

// global define ==============================================================

//static const char *TAG = "MT_MODULE_UNARYCALL";

// global func ================================================================

esp_err_t mt_module_unarycall_sys_dispatch(
    Ai__Metathings__Component__DownStreamFrame *msg, char *module_id) {
  int32_t match = 0;

  // config api
  if (strcmp(msg->unary_call->method->value, "SysConfigGetIntConfig") == 0) {
    mt_module_unarycall_config_get_int32_config_handle(msg, module_id);
    match++;
    goto EXIT;
  }

  if (strcmp(msg->unary_call->method->value, "SysConfigSetIntConfig") == 0) {
    mt_module_unarycall_config_set_int32_config_handle(msg, module_id);
    match++;
    goto EXIT;
  }

  if (strcmp(msg->unary_call->method->value, "SysConfigGetStringConfig") == 0) {
    mt_module_unarycall_config_get_string_config_handle(msg, module_id);
    match++;
    goto EXIT;
  }

  if (strcmp(msg->unary_call->method->value, "SysConfigSetStringConfig") == 0) {
    mt_module_unarycall_config_set_string_config_handle(msg, module_id);
    match++;
    goto EXIT;
  }

  if (strcmp(msg->unary_call->method->value, "SysConfigGetHostConfig") == 0) {
    mt_module_unarycall_config_get_host_config_handle(msg, module_id);
    match++;
    goto EXIT;
  }

  if (strcmp(msg->unary_call->method->value, "SysConfigSetHostConfig") == 0) {
    mt_module_unarycall_config_set_host_config_handle(msg, module_id);
    match++;
    goto EXIT;
  }

  // monitor api
  if (strcmp(msg->unary_call->method->value, "SysMonitorGetState") == 0) {
    mt_module_unarycall_monitor_get_state_handle(msg, module_id);
    match++;
    goto EXIT;
  }

  if (strcmp(msg->unary_call->method->value, "SysMonitorSetRestart") == 0) {
    mt_module_unarycall_monitor_set_restart_handle(msg, module_id);
    match++;
    goto EXIT;
  }

  // ota api
  if (strcmp(msg->unary_call->method->value, "GetVersion") == 0) {
    mt_module_unarycall_version_handle(msg, module_id);
    match++;
    goto EXIT;
  }

  if (strcmp(msg->unary_call->method->value, "OtaUpdate") == 0) {
    mt_module_unarycall_ota_handle(msg, module_id);
    match++;
    goto EXIT;
  }

EXIT:
  if (match > 0)
    return ESP_OK;
  else {
    return ESP_ERR_INVALID_ARG;
  }
}
