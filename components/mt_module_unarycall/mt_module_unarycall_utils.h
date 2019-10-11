#ifndef _MT_MODULE_UNARYCALL_UTILS_H
#define _MT_MODULE_UNARYCALL_UTILS_H

#include "google/protobuf/any.pb-c.h"
#include "google/protobuf/empty.pb-c.h"
#include "stream_frame.pb-c.h"

uint8_t *mt_module_unarycall_utils_pack(
    uint8_t *buf_in, int size_in, const char *methodres_in,
    Ai__Metathings__Component__DownStreamFrame *msg_in, int *frame_size_out);

#endif
