#ifndef _MT_MODULE_FLOW_H
#define _MT_MODULE_FLOW_H

#include "esp_err.h"
#include "mt_module_http.h"
#include "stdint.h"

typedef struct _module_flow_t
{
    int module_index;
    int flow_index;
    mt_module_http_t *module_http;
    flow_t *flow;
    char *session;

    uint32_t create_push_frame_interval;
    uint32_t push_frame_interval;
    uint32_t ping_interval;
    uint8_t ping_retry_times;
    uint8_t ping_count;
    bool push_ack;
    bool config_ack;

    bool data_ack;
    char *data_id;

} mt_module_flow_t;

void mt_module_flow_process(mt_module_flow_t *module_flow, char *topic,
                            uint8_t *buf, uint8_t size);

void mt_module_flow_task(mt_module_flow_t *module_flow, char *task_name);

mt_module_flow_t *mt_module_flow_new(int module_index, int flow_index,
                                     mt_module_http_t *module_http);

#endif