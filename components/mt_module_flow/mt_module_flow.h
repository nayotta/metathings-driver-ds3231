#ifndef _MT_MODULE_FLOW_H
#define _MT_MODULE_FLOW_H

#include "esp_err.h"
#include "mt_module_http.h"
#include "stdint.h"
#include "struct.pb-c.h"

// define =====================================================================

typedef struct _mt_module_flow_struct_t {
  char *key;
  Google__Protobuf__Value__KindCase type;
  union {
    double number_value;
    char *string_value;
    protobuf_c_boolean bool_value;
  };
} mt_module_flow_struct_t;

typedef struct _module_struct_group_t {
  int size;
  mt_module_flow_struct_t **value;
} mt_module_flow_struct_group_t;

typedef struct _module_flow_t {
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

// help func ==================================================================

mt_module_flow_struct_group_t *mt_module_flow_new_struct_group(int size);

mt_module_flow_struct_group_t *
mt_module_flow_new_struct_group_with_notify(int size);

void mt_module_flow_free_struct_group(mt_module_flow_struct_group_t *group);

void mt_module_flow_free_struct_group(mt_module_flow_struct_group_t *value);

uint8_t *mt_module_flow_pack_frame(mt_module_flow_struct_group_t *value_in,
                                   char *session_id, int *size_out);

uint8_t *mt_module_flow_repack_frame_with_session(uint8_t *buf_in, int size_in,
                                                  char *session_id,
                                                  int *size_out);

// func =======================================================================

void mt_module_flow_process(mt_module_flow_t *module_flow, char *topic,
                            uint8_t *buf, uint8_t size);

void mt_module_flow_task(mt_module_flow_t *module_flow, char *task_name);

mt_module_flow_t *mt_module_flow_new(int module_index, int flow_index,
                                     mt_module_http_t *module_http);

esp_err_t mt_module_flow_sent_msg(mt_module_flow_t *module_flow,
                                  mt_module_flow_struct_group_t *msg);

#endif
