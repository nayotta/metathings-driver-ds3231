#ifndef MT_MODULE_HTTP_H
#define MT_MODULE_HTTP_H

#include "esp_err.h"
#include "esp_http_client.h"
#include "stdint.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

typedef struct _module_t
{
  char *id;
  char *state;
  char *deviceID;
  char *endpoint;
  char *component;
  char *name;
  char *alias;
  int *heartbeat_at;
} module_t;

typedef struct _flow_t
{
  char *id;
  char *device_id;
  char *name;
  char *alias;
} flow_t;

typedef struct _device_t
{
  char *id;
  char *kind;
  char *state;
  char *name;
  char *alias;
  uint8_t *modules;
  int module_num;
  int *heartbeat_at;
  uint8_t *flows;
  int flow_num;
} device_t;

typedef struct _object_t
{
  device_t *device;
  char *prefix;
  char *name;
  int length;
  char *etag;
  int last_modified;
} object_t;

typedef struct _domain_t
{
  char *id;
  char *name;
  char *alias;
} domain_t;

typedef struct _action_t
{
  char *id;
  char *name;
  char *alias;
  char *description;
} action_t;

typedef struct _role_t
{
  char *id;
  char *name;
  char *alias;
  uint8_t *actions;
  int actions_num;
} role_t;

typedef struct _group_t
{
  char *id;
  uint8_t *domains;
  int domains_num;
  uint8_t *roles;
  int roles_num;
  char *name;
  char *alias;
  char *description;
} group_t;

typedef struct _entity_t
{
  char *id;
  uint8_t *entities;
  int entities_num;
  uint8_t *groups;
  int groups_num;
  uint8_t *roles;
  int roles_num;
  char *name;
  char *password;
  char *extra;
} entity_t;

typedef struct _credential_t
{
  char *id;
  uint8_t *domains;
  int domains_num;
  uint8_t *roles;
  int rolse_num;
  uint8_t *entities;
  int entities_num;
  char *name;
  char *alias;
  char *secret;
  char *description;
  char *expores_at;
} credential_t;

typedef struct _token_t
{
  char *id;
  int *issued_at;
  entity_t *entity;
  uint8_t *roles;
  int roles_num;
  domain_t *domain;
  credential_t *credential;
  char *text;
  uint8_t *groups;
  int groups_num;
} token_t;

typedef struct _push_frame_res_t
{
  char *id;
  char *sesssion_id;
} push_frame_res_t;

typedef struct _mt_module_http_t
{
  char *host;
  int port;
  credential_t *cred;
  char *token;
  module_t *module;
  uint64_t session_id;
  http_event_handle_cb event_handler;
  char *response_content;
  int response_content_size;
} mt_module_http_t;

esp_err_t mt_module_http_actions_issue_module_token(
    mt_module_http_t *module_http);

module_t *mt_module_http_actions_show_module(mt_module_http_t *module_http);

esp_err_t mt_module_http_actions_heartbeat(mt_module_http_t *module_http,
                                           module_t *mod_in);

esp_err_t mt_module_http_actions_put_object(mt_module_http_t *module_http,
                                            object_t *obj_in, char *content_in);

esp_err_t mt_module_http_actions_remove_object(mt_module_http_t *module_http,
                                               object_t *obj_in);

esp_err_t mt_module_http_actions_rename_object(mt_module_http_t *module_http,
                                               object_t *src_in,
                                               object_t *des_in);

object_t *mt_module_http_actions_get_object(mt_module_http_t *module_http,
                                            object_t *obj_in);

char *mt_module_http_actions_get_object_content(mt_module_http_t *module_http,
                                                object_t *obj_in);

uint8_t *mt_module_http_actions_list_objects(mt_module_http_t *module_http,
                                             object_t *obj_in, int *obj_num);

push_frame_res_t *mt_module_http_actions_push_frame_to_flow(
    mt_module_http_t *module_http, flow_t *flow_in, bool config_ack_in,
    bool push_ack_in);

void mt_module_http_task(mt_module_http_t *module_http, char *task_name);

mt_module_http_t *mt_module_http_new(int module_index);

#endif