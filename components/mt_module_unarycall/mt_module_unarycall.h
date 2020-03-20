#ifndef _MT_MODULE_UNARYCALL_H
#define _MT_MODULE_UNARYCALL_H

#include "esp_err.h"
#include "stream_frame.pb-c.h"

esp_err_t mt_module_unarycall_sys_dispatch(
    Ai__Metathings__Component__DownStreamFrame *msg, char *module_id);

#endif
