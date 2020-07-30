#ifndef _MT_MODULE_UNARYCALL_UTILS_H
#define _MT_MODULE_UNARYCALL_UTILS_H

#include "esp_err.h"
#include "google/protobuf/any.pb-c.h"
#include "google/protobuf/empty.pb-c.h"
#include "stream_frame.pb-c.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

esp_err_t mt_module_unarycall_utils_check(
    Ai__Metathings__Component__DownStreamFrame *msg);

uint8_t *mt_module_unarycall_utils_pack(
    uint8_t *buf_in, int size_in, const char *methodres_in,
    Ai__Metathings__Component__DownStreamFrame *msg_in, int *frame_size_out);

esp_err_t mt_module_unarycall_utils_mqtt_sent_msg(
    uint8_t *buf, int32_t size, char *module_id, const char *method,
    Ai__Metathings__Component__DownStreamFrame *msg);
#endif
