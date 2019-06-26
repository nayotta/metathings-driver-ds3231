#ifndef MT_MODULE_HTTP_H
#define MT_MODULE_HTTP_H

#include "esp_err.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

typedef struct module_t
{
  char *id;
  char *state;
  char *endpoint;
  char *component;
  char *name;
  char *alias;
  char *heartbeat_at;
};

typedef struct flow_t
{
  char *id;
  char *device_id;
  char *name;
  char *alias;
};

typedef struct device_t
{
  char *id;
  char *kind;
  char *state;
  char *name;
  char *alias;
  struct module_t *modules;
  char *heartbeat_at;
  struct flow_t *flows;
};

typedef struct object_t
{
  struct device_t *device;
  char *prefix;
  char *name;
  int length;
  char *etag;
  int last_modified;
};

typedef struct domain_t
{
  char *id;
  char *name;
  char *alias;
};

typedef struct action_t
{
  char *id;
  char *name;
  char *alias;
  char *description;
};

typedef struct role_t
{
  char *id;
  char *name;
  char *alias;
  struct action_t *actions;
};

typedef struct group_t
{
  char *id;
  struct domain_t *domains;
  struct role_t *roles;
  char *name;
  char *alias;
  char *description;
};

typedef struct entity_t
{
  char *id;
};

typedef struct credential_t
{
  char *id;
  struct domain_t *domains;
  struct role_t *roles;
  struct entity_t *entities;
  char *name;
  char *alias;
  char *sectret;
  char *description;
  char *expores_at;
};

typedef struct token_t
{
  char *id;
  char *issued_at;
};

esp_err_t mt_module_http_actions_issue_module_token(
    struct credential_t *cred_in, int timestamp, int nonce, char *hmac,
    struct token_t *tkn_out);

esp_err_t mt_module_http_actions_show_module(char *token_in,
                                             struct module_t *mdl_out);

esp_err_t mt_module_http_actions_heartbeat(char *token_in,
                                           struct module_t *mod_in);

esp_err_t mt_module_http_actions_put_object(char *token_in,
                                            struct object_t *obj_in,
                                            char *content_in);

esp_err_t mt_module_http_actions_remove_object(char *token_in,
                                               struct object_t *obj_in);

esp_err_t mt_module_http_actions_rename_object(char *token_in,
                                               struct object_t *src_in,
                                               struct object_t *des_in);

esp_err_t mt_module_http_acitons_get_object(char *token_in,
                                            struct object_t *obj_in,
                                            struct object_t *obj_out);

esp_err_t mt_module_http_actions_get_object_content(char *token_in,
                                                    struct object_t *obj_in,
                                                    char *content_out);

esp_err_t mt_module_http_actions_list_objects(char *token_in,
                                              struct object_t *obj_in,
                                              struct object_t **objs_out);

#endif