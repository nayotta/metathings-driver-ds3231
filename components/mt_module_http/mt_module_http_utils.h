#ifndef MT_MODULE_HTTP_UTILS_H
#define MT_MODULE_HTTP_UTILS_H

#include "esp_err.h"
#include "mt_module_http.h"

// help func ==================================================================

char *mt_utils_login_get_issue_token_data(mt_module_http_t *module_http);

char *mt_utils_login_get_heartbeat_data(mt_module_http_t *module_http);

// global func ================================================================

token_t *mt_module_http_utils_token_t_init();

module_t *mt_module_http_utils_motule_t_init();

esp_err_t mt_module_http_utils_free_module(module_t *module);

esp_err_t mt_module_http_utils_free_flow(flow_t *flow);

esp_err_t mt_module_http_utils_free_push_frame_res(push_frame_res_t *res);

esp_err_t mt_module_http_utils_free_device(device_t *device);

esp_err_t mt_module_http_utils_free_object(object_t *object);

esp_err_t mt_module_http_utils_free_domain(domain_t *domain);

esp_err_t mt_module_http_utils_free_action(action_t *action);

esp_err_t mt_module_http_utils_free_role(role_t *role);

esp_err_t mt_module_http_utils_free_group(group_t *group);

esp_err_t mt_module_http_utils_free_entity(entity_t *entity);

esp_err_t mt_module_http_utils_free_credenital(credential_t *credential);

esp_err_t mt_module_http_utils_free_token(token_t *token);

esp_err_t mt_module_http_utils_free_objects(uint8_t *objects, int num);

token_t *mt_module_http_utils_parse_token_res(char *content_in);

module_t *mt_module_http_uitls_parse_module_res(char *content_in);

push_frame_res_t *mt_module_http_utils_parse_push_frame_res(char *content_in);

object_t *mt_module_http_utils_parse_object_res(char *content_in);

char *mt_module_http_utils_parse_content_res(char *content_in);

uint8_t *mt_module_http_utils_parse_objects_res(char *content_in,
                                                int *objs_num);

#endif
