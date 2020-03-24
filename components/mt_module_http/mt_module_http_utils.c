#include "stdlib.h"
#include "string.h"

#include "esp_err.h"
#include "esp_log.h"

#include "cJSON.h"

#include "mt_http_client.h"
#include "mt_module_http.h"
#include "mt_utils.h"

// global define ==============================================================
static const char *TAG = "MT_MODULE_HTTP_UTILS";

// global func ================================================================
token_t *mt_module_http_utils_token_t_init() {
  token_t *token = NULL;

  token = malloc(sizeof(token_t));
  token->id = NULL;
  token->issued_at = 0;
  token->entity = NULL;
  token->roles = NULL;
  token->roles_num = 0;
  token->domain = NULL;
  token->credential = NULL;
  token->text = NULL;
  token->groups = NULL;
  token->groups_num = 0;

  return token;
}

module_t *mt_module_http_utils_motule_t_init() {
  module_t *module = NULL;

  module = malloc(sizeof(module_t));
  module->id = NULL;
  module->state = NULL;
  module->deviceID = NULL;
  module->endpoint = NULL;
  module->component = NULL;
  module->name = NULL;
  module->alias = NULL;
  module->heartbeat_at = NULL;

  return module;
}

push_frame_res_t *mt_module_http_utils_push_frame_res_t_init() {
  push_frame_res_t *res = NULL;

  res = malloc(sizeof(push_frame_res_t));
  res->id = NULL;
  res->session_id = NULL;

  return res;
}

esp_err_t mt_module_http_utils_free_module(module_t *module) {
  if (module == NULL) {
    ESP_LOGE(TAG, "%4d %s NULL, no need to free", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (module->id != NULL)
    free(module->id);

  if (module->state != NULL)
    free(module->state);

  if (module->deviceID != NULL)
    free(module->deviceID);

  if (module->endpoint != NULL)
    free(module->endpoint);

  if (module->component != NULL)
    free(module->component);

  if (module->name != NULL)
    free(module->name);

  if (module->alias != NULL)
    free(module->alias);

  if (module->heartbeat_at != NULL)
    free(module->heartbeat_at);

  free(module);

  return ESP_OK;
}

esp_err_t mt_module_http_utils_free_push_frame_res(push_frame_res_t *res) {
  if (res == NULL) {
    ESP_LOGE(TAG, "%4d %s NULL, no need to free", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (res->id != NULL)
    free(res->id);

  if (res->session_id != NULL)
    free(res->session_id);

  free(res);

  return ESP_OK;
}

esp_err_t mt_module_http_utils_free_flow(flow_t *flow) {
  if (flow == NULL) {
    ESP_LOGE(TAG, "%4d %sNULL, no need to free", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (flow->id != NULL)
    free(flow->id);

  if (flow->device_id != NULL)
    free(flow->device_id);

  if (flow->name != NULL)
    free(flow->name);

  if (flow->alias != NULL)
    free(flow->alias);

  free(flow);

  return ESP_OK;
}

esp_err_t mt_module_http_utils_free_device(device_t *device) {
  if (device == NULL) {
    ESP_LOGE(TAG, "%4d %sNULL, no need to free", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (device->id != NULL)
    free(device->id);

  if (device->kind != NULL)
    free(device->kind);

  if (device->state != NULL)
    free(device->state);

  if (device->name != NULL)
    free(device->name);

  if (device->alias != NULL)
    free(device->alias);

  if (device->modules != NULL)
    free(device->modules);

  if (device->flows != NULL)
    free(device->flows);

  free(device);

  return ESP_OK;
}

esp_err_t mt_module_http_utils_free_object(object_t *object) {
  if (object == NULL) {
    ESP_LOGE(TAG, "%4d %sNULL, no need to free", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (object->device != NULL) {
    mt_module_http_utils_free_device(object->device);
  }

  if (object->prefix != NULL)
    free(object->prefix);

  if (object->name != NULL)
    free(object->name);

  if (object->etag != NULL)
    free(object->etag);

  free(object);

  return ESP_OK;
}

esp_err_t mt_module_http_utils_free_domain(domain_t *domain) {
  if (domain == NULL) {
    ESP_LOGE(TAG, "%4d %sNULL, no need to free", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (domain->id != NULL)
    free(domain->id);

  if (domain->name != NULL)
    free(domain->name);

  if (domain->alias != NULL)
    free(domain->alias);

  free(domain);

  return ESP_OK;
}

esp_err_t mt_module_http_utils_free_action(action_t *action) {
  if (action == NULL) {
    ESP_LOGE(TAG, "%4d %sNULL, no need to free", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (action->id != NULL)
    free(action->id);

  if (action->name != NULL)
    free(action->name);

  if (action->alias != NULL)
    free(action->alias);

  if (action->description != NULL)
    free(action->description);

  free(action);

  return ESP_OK;
}

esp_err_t mt_module_http_utils_free_role(role_t *role) {
  if (role == NULL) {
    ESP_LOGE(TAG, "%4d %sNULL, no need to free", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (role->id != NULL)
    free(role->id);

  if (role->name != NULL)
    free(role->name);

  if (role->alias != NULL)
    free(role->alias);

  if (role->actions != NULL)
    free(role->actions);

  free(role);

  return ESP_OK;
}

esp_err_t mt_module_http_utils_free_group(group_t *group) {
  if (group == NULL) {
    ESP_LOGE(TAG, "%4d %sNULL, no need to free", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (group->id != NULL)
    free(group->id);

  if (group->domains != NULL)
    free(group->domains);

  if (group->roles != NULL)
    free(group->roles);

  if (group->name != NULL)
    free(group->name);

  if (group->alias != NULL)
    free(group->alias);

  if (group->description != NULL)
    free(group->description);

  free(group);

  return ESP_OK;
}

esp_err_t mt_module_http_utils_free_entity(entity_t *entity) {
  if (entity == NULL) {
    ESP_LOGE(TAG, "%4d %sNULL, no need to free", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (entity->id != NULL)
    free(entity->id);

  if (entity->entities != NULL)
    free(entity->entities);

  if (entity->groups != NULL)
    free(entity->groups);

  if (entity->roles != NULL)
    free(entity->roles);

  if (entity->name != NULL)
    free(entity->name);

  if (entity->password != NULL)
    free(entity->password);

  if (entity->extra != NULL)
    free(entity->extra);

  free(entity);

  return ESP_OK;
}

esp_err_t mt_module_http_utils_free_credenital(credential_t *credential) {
  if (credential == NULL) {
    ESP_LOGE(TAG, "%4d %sNULL, no need to free", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (credential->id != NULL)
    free(credential->id);

  if (credential->domains != NULL)
    free(credential->domains);

  if (credential->roles != NULL)
    free(credential->roles);

  if (credential->entities != NULL)
    free(credential->entities);

  if (credential->name != NULL)
    free(credential->name);

  if (credential->alias != NULL)
    free(credential->alias);

  if (credential->secret != NULL)
    free(credential->secret);

  if (credential->description != NULL)
    free(credential->description);

  if (credential->expores_at != NULL)
    free(credential->expores_at);

  free(credential);

  return ESP_OK;
}

esp_err_t mt_module_http_utils_free_token(token_t *token) {
  if (token == NULL) {
    ESP_LOGE(TAG, "%4d %s NULL, no need to free", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (token->id != NULL)
    free(token->id);

  if (token->issued_at != NULL)
    free(token->issued_at);

  if (token->entity != NULL)
    mt_module_http_utils_free_entity(token->entity);

  if (token->roles != NULL)
    free(token->roles);

  if (token->domain != NULL)
    free(token->domain);

  if (token->credential != NULL)
    mt_module_http_utils_free_credenital(token->credential);

  if (token->text != NULL)
    free(token->text);

  if (token->groups != NULL)
    free(token->groups);

  free(token);
  token = NULL;

  return ESP_OK;
}

esp_err_t mt_module_http_utils_free_objects(uint8_t *objects, int num) {
  esp_err_t err;
  object_t *obj;

  for (int i = 0; i < num; i++) {
    obj = (object_t *)(objects + sizeof(object_t) * i);
    err = mt_module_http_utils_free_object(obj);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d mt_module_http_utils_free_object err", __LINE__);
    }
  }

  free(objects);

  return ESP_OK;
}

token_t *mt_module_http_utils_parse_token_res(char *content_in) {
  esp_err_t err = ESP_OK;
  cJSON *root = NULL, *item = NULL, *tkn_item = NULL;
  token_t *tkn_out = mt_module_http_utils_token_t_init();

  root = cJSON_Parse(content_in);
  if (root == NULL) {
    ESP_LOGE(TAG, "%4d cJSON_Parse NULL", __LINE__);
    err = ESP_ERR_HTTP_BASE;
    goto ERROR;
  }
  for (int i = 0; i < cJSON_GetArraySize(root); i++) {
    item = cJSON_GetArrayItem(root, i);
    if (cJSON_Object == item->type) {
      if (strcmp(item->string, "token") == 0) {
        for (int j = 0; j < cJSON_GetArraySize(item); j++) {
          tkn_item = cJSON_GetArrayItem(item, j);
          if (cJSON_String == tkn_item->type) {
            if (strcmp(tkn_item->string, "text") == 0) {
              tkn_out->text = mt_utils_string_copy(tkn_item->valuestring);
            }
          }
        }
      }
    }
  }

ERROR:
  cJSON_Delete(root);

  if (err != ESP_OK) {
    if (tkn_out != NULL) {
      mt_module_http_utils_free_token(tkn_out);
    }
    return NULL;
  }
  return tkn_out;
}

module_t *mt_module_http_uitls_parse_module_res(char *content_in) {
  esp_err_t err = ESP_OK;
  cJSON *root = NULL, *item = NULL, *mdl_item = NULL;
  module_t *mdl_out = mt_module_http_utils_motule_t_init();

  root = cJSON_Parse(content_in);
  if (root == NULL) {
    ESP_LOGE(TAG, "%4d cJSON_Parse NULL", __LINE__);
    err = ESP_ERR_HTTP_BASE;
    goto ERROR;
  }
  for (int i = 0; i < cJSON_GetArraySize(root); i++) {
    item = cJSON_GetArrayItem(root, i);

    if (cJSON_Object == item->type) {
      if (strcmp(item->string, "module") == 0) {
        for (int j = 0; j < cJSON_GetArraySize(item); j++) {
          mdl_item = cJSON_GetArrayItem(item, j);
          if (cJSON_String == mdl_item->type) {
            if (strcmp(mdl_item->string, "id") == 0)
              mdl_out->id = mt_utils_string_copy(mdl_item->valuestring);

            if (strcmp(mdl_item->string, "state") == 0)
              mdl_out->state = mt_utils_string_copy(mdl_item->valuestring);

            if (strcmp(mdl_item->string, "deviceId") == 0)
              mdl_out->deviceID = mt_utils_string_copy(mdl_item->valuestring);

            if (strcmp(mdl_item->string, "endpoint") == 0)
              mdl_out->endpoint = mt_utils_string_copy(mdl_item->valuestring);

            if (strcmp(mdl_item->string, "component") == 0)
              mdl_out->component = mt_utils_string_copy(mdl_item->valuestring);

            if (strcmp(mdl_item->string, "name") == 0)
              mdl_out->name = mt_utils_string_copy(mdl_item->valuestring);

            if (strcmp(mdl_item->string, "alias") == 0)
              mdl_out->alias = mt_utils_string_copy(mdl_item->valuestring);
          }
          if (cJSON_Number == mdl_item->type) {
            if (strcmp(mdl_item->string, "heartbeat_at") == 0) {
              *mdl_out->heartbeat_at = mdl_item->valueint;
            }
          }
        }
      }
    }
  }

ERROR:
  cJSON_Delete(root);

  if (err != ESP_OK) {
    if (mdl_out != NULL) {
      mt_module_http_utils_free_module(mdl_out);
    }
    return NULL;
  }
  return mdl_out;
}

push_frame_res_t *mt_module_http_utils_parse_push_frame_res(char *content_in) {
  esp_err_t err = ESP_OK;
  cJSON *root = NULL, *item = NULL, *config_item = NULL;
  push_frame_res_t *res_out = mt_module_http_utils_push_frame_res_t_init();

  root = cJSON_Parse(content_in);
  if (root == NULL) {
    ESP_LOGE(TAG, "%4d cJSON_Parse NULL", __LINE__);
    err = ESP_ERR_HTTP_BASE;
    goto ERROR;
  }
  for (int i = 0; i < cJSON_GetArraySize(root); i++) {
    item = cJSON_GetArrayItem(root, i);

    if (cJSON_String == item->type) {

      if (strcmp(item->string, "id") == 0)
        res_out->id = mt_utils_string_copy(item->valuestring);
    }

    if (item->type == cJSON_Object) {
      if (strcmp(item->string, "config") == 0) {
        for (int j = 0; j < cJSON_GetArraySize(item); j++) {
          config_item = cJSON_GetArrayItem(item, j);
          if (cJSON_String == config_item->type) {
            if (strcmp(config_item->string, "session") == 0)
              res_out->session_id =
                  mt_utils_string_copy(config_item->valuestring);
          }
        }
      }
    }
  }

ERROR:
  cJSON_Delete(root);

  if (err != ESP_OK) {
    if (res_out != NULL) {
      mt_module_http_utils_free_push_frame_res(res_out);
    }
    return NULL;
  }
  return res_out;
}

object_t *mt_module_http_utils_parse_object_res(char *content_in) {
  esp_err_t err = ESP_OK;

  cJSON *root = NULL, *item = NULL, *item_device = NULL;
  object_t *obj_out = malloc(sizeof(object_t));

  root = cJSON_Parse(content_in);
  if (root == NULL) {
    ESP_LOGE(TAG, "%4d cJSON_Parse NULL", __LINE__);
    err = ESP_ERR_HTTP_BASE;
    goto ERROR;
  }

  for (int i = 0; i < cJSON_GetArraySize(root); i++) {
    item = cJSON_GetArrayItem(root, i);
    if (cJSON_Object == item->type) {
      if (strcmp(item->string, "device") == 0) {
        for (int j = 0; j < cJSON_GetArraySize(item); j++) {
          item_device = cJSON_GetArrayItem(item, j);
          if (cJSON_String == item_device->type) {
            if (strcmp(item_device->string, "id") == 0) {
              obj_out->device->id = item_device->valuestring;
            }

            if (strcmp(item_device->string, "kind") == 0) {
              obj_out->device->kind = item_device->valuestring;
            }

            if (strcmp(item_device->string, "state") == 0) {
              obj_out->device->state = item_device->valuestring;
            }

            if (strcmp(item_device->string, "name") == 0) {
              obj_out->device->name = item_device->valuestring;
            }

            if (strcmp(item_device->string, "alias") == 0) {
              obj_out->device->alias = item_device->valuestring;
            }
          }

          if (cJSON_Number == item_device->type) {
            if (strcmp(item_device->string, "heartbeat_at") == 0) {
              *obj_out->device->heartbeat_at = item_device->valueint;
            }
          }
        }
      }
    }

    if (cJSON_String == item->type) {
      if (strcmp(item_device->string, "prefix") == 0) {
        obj_out->prefix = item_device->valuestring;
      }

      if (strcmp(item_device->string, "name") == 0) {
        obj_out->name = item_device->valuestring;
      }
    }

    if (cJSON_Number == item->type) {
      if (strcmp(item_device->string, "length") == 0) {
        obj_out->length = item_device->valueint;
      }

      if (strcmp(item_device->string, "last_modified") == 0) {
        obj_out->last_modified = item_device->valueint;
      }
    }
  }

ERROR:
  cJSON_Delete(root);

  if (err != ESP_OK) {
    if (obj_out != NULL) {
      mt_module_http_utils_free_object(obj_out);
    }

    return NULL;
  } else {
    return obj_out;
  }
}

char *mt_module_http_utils_parse_content_res(char *content_in) {
  esp_err_t err = ESP_OK;
  char *content_out = NULL;

  cJSON *root = NULL, *item = NULL;

  root = cJSON_Parse(content_in);
  if (root == NULL) {
    ESP_LOGE(TAG, "%4d cJSON_Parse NULL", __LINE__);
    err = ESP_ERR_HTTP_BASE;
    goto ERROR;
  }

  for (int i = 0; i < cJSON_GetArraySize(root); i++) {
    item = cJSON_GetArrayItem(root, i);
    if (cJSON_String == item->type) {
      if (strcmp(item->string, "content") == 0) {
        content_out = item->valuestring;
      }
    }
  }

ERROR:
  cJSON_Delete(root);

  if (item != NULL) {
    cJSON_Delete(item);
  }

  if (err != ESP_OK) {
    if (content_out != NULL)
      free(content_out);

    return NULL;
  }

  return content_out;
}

uint8_t *mt_module_http_utils_parse_objects_res(char *content_in,
                                                int *objs_num) {
  esp_err_t err = ESP_OK;
  objs_num = 0;
  uint8_t *objs_out = NULL;

  cJSON *root = NULL, *item = NULL, *item_obj = NULL;

  root = cJSON_Parse(content_in);
  if (root == NULL) {
    ESP_LOGE(TAG, "%4d cJSON_Parse NULL", __LINE__);
    err = ESP_ERR_HTTP_BASE;
    goto ERROR;
  }

  for (int i = 0; i < cJSON_GetArraySize(root); i++) {
    item = cJSON_GetArrayItem(root, i);
    if (cJSON_Object == item->type) {
      if (strcmp(item->string, "objects") == 0) {
        *objs_num = cJSON_GetArraySize(item);
        objs_out = malloc(sizeof(object_t) * (*objs_num));

        for (int j = 0; j < *objs_num; j++) {
          object_t *obj_temp = NULL;
          item_obj = cJSON_GetArrayItem(item, j);
          obj_temp =
              mt_module_http_utils_parse_object_res(cJSON_Print(item_obj));
          if (obj_temp == NULL) {
            err = ESP_ERR_HTTP_BASE;
            ESP_LOGE(TAG,
                     "%4d mt_module_http_utils_parse_object_res failed code=%d",
                     __LINE__, err);
            goto ERROR;
          }
          memcpy(objs_out + sizeof(object_t) * j, obj_temp, sizeof(object_t));
        }
      }
    }
  }

ERROR:
  cJSON_Delete(root);

  if (err != ESP_OK) {
    if (objs_out != NULL) {
      mt_module_http_utils_free_objects(objs_out, *objs_num);
    }
    return NULL;
  }

  return objs_out;
}
