#ifndef _MT_MODULE_UNARYCALL_CONFIG_H
#define _MT_MODULE_UNARYCALL_CONFIG_H

#include "esp_err.h"
#include "stream_frame.pb-c.h"

void mt_module_unarycall_config_get_int32_config_handle(
    Ai__Metathings__Component__DownStreamFrame *msg, char module_id[128]);

void mt_module_unarycall_config_set_int32_config_handle(
    Ai__Metathings__Component__DownStreamFrame *msg, char module_id[128]);

void mt_module_unarycall_config_get_string_config_handle(
    Ai__Metathings__Component__DownStreamFrame *msg, char module_id[128]);

void mt_module_unarycall_config_set_string_config_handle(
    Ai__Metathings__Component__DownStreamFrame *msg, char module_id[128]);

void mt_module_unarycall_config_get_host_config_handle(
    Ai__Metathings__Component__DownStreamFrame *msg, char module_id[128]);

void mt_module_unarycall_config_set_host_config_handle(
    Ai__Metathings__Component__DownStreamFrame *msg, char module_id[128]);

#endif
