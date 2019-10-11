#ifndef _MT_MODULE_UNARYCALL_OTA_H
#define _MT_MODULE_UNARYCALL_OTA_H

#include "esp_err.h"
#include "stream_frame.pb-c.h"

void mt_module_unarycall_version_handle(
    Ai__Metathings__Component__DownStreamFrame *msg, char module_id[128]);

void mt_module_unarycall_ota_handle(
    Ai__Metathings__Component__DownStreamFrame *msg, char module_id[128]);

#endif
