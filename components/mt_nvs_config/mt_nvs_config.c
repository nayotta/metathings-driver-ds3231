#include "esp_log.h"
#include "stdlib.h"
#include "string.h"

#include "mt_nvs_config.h"
#include "mt_nvs_storage.h"
#include "mt_utils_string.h"

// global config ==============================================================

static const char *TAG = "MT_NVS_CONFIG";

static const int DEFAULT_FLOW_INTERVAL = 600; // 10 minute

// help func ==================================================================

mt_nvs_host_t *mt_nvs_config_new_host() {
  mt_nvs_host_t *host = malloc(sizeof(mt_nvs_host_t));

  host->host = NULL;
  host->http_port = 0;
  host->mqtt_port = 0;
  host->net_type = NULL;
  host->use_ssl = false;

  return host;
}

mt_nvs_flows_t *mt_nvs_config_new_flows() {
  mt_nvs_flows_t *flows = malloc(sizeof(mt_nvs_flows_t));

  flows->flows = NULL;
  flows->flow_num = 0;

  return flows;
}

mt_nvs_module_t *mt_nvs_config_new_module() {
  mt_nvs_module_t *module = malloc(sizeof(mt_nvs_module_t));

  module->flows = NULL;
  module->id = NULL;
  module->index = 0;
  module->key = NULL;
  module->name = NULL;

  return module;
}

void mt_nvs_config_free_host(mt_nvs_host_t *host) {
  if (host == NULL)
    return;

  if (host->host != NULL)
    free(host->host);

  if (host->mqtt_port != NULL)
    free(host->mqtt_port);

  if (host->net_type != NULL)
    free(host->net_type);

  free(host);
}

void mt_nvs_config_free_flows(mt_nvs_flows_t *flows) {
  if (flows == NULL)
    return;

  if (flows->flows != NULL) {
    for (int i = 0; i < flows->flow_num; i++) {
      if (flows->flows[i] != NULL)
        free(flows->flows[i]);
    }

    free(flows->flows);
  }

  free(flows);
}

void mt_nvs_config_free_module(mt_nvs_module_t *module) {
  if (module == NULL)
    return;

  if (module->id != NULL)
    free(module->id);

  if (module->key != NULL)
    free(module->key);

  if (module->name != NULL)
    free(module->name);

  if (module->flows != NULL)
    mt_nvs_config_free_flows(module->flows);

  free(module);
}

// global func ================================================================

mt_nvs_host_t *mt_nvs_config_get_host_config() {
  esp_err_t err = ESP_OK;
  size_t size = 0;
  mt_nvs_host_t *host_out = malloc(sizeof(mt_nvs_host_t));
  char *net_type = NULL;

  // get host_out->host
  host_out->host = mt_nvs_read_string_config("host", &size);
  if (host_out->host == NULL) {
    ESP_LOGE(TAG, "%4d %s get host failed", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // get host_out->mqtt_port
  host_out->mqtt_port = mt_nvs_read_string_config("mqtt_port", &size);
  if (host_out->mqtt_port == NULL) {
    ESP_LOGE(TAG, "%4d %s get mqtt_port failed", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // get host_out->http_port
  if (mt_nvs_read_int32_config("http_port", &host_out->http_port) == false) {
    ESP_LOGE(TAG, "%4d %s get http_port failed", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // get host_out->use_ssl
  int32_t use_ssl = 0;
  if (mt_nvs_read_int32_config("use_ssl", &use_ssl) == false) {
    ESP_LOGE(TAG, "%4d %s get use_ssl failed", __LINE__, __func__);
    use_ssl = 0;
  }
  if (use_ssl == 0)
    host_out->use_ssl = false;
  else
    host_out->use_ssl = true;

  // get host_out->net_type
  size_t net_type_len = 0;
  net_type = mt_nvs_read_string_config("net_type", &net_type_len);
  host_out->net_type = malloc(12);
  if (net_type == NULL) {
    ESP_LOGE(TAG, "%4d %s get net_type failed", __LINE__, __func__);
    sprintf(host_out->net_type, "wifi");
  } else {
    if (strcmp(net_type, "eth") == 0)
      sprintf(host_out->net_type, "eth");
    else if (strcmp(net_type, "wifi") == 0)
      sprintf(host_out->net_type, "wifi");
    else if (strcmp(net_type, "4g") == 0)
      sprintf(host_out->net_type, "4g");
    else
      sprintf(host_out->net_type, "wifi");
  }

EXIT:
  if (err != ESP_OK) {
    mt_nvs_config_free_host(host_out);
    host_out = NULL;
  }
  if (net_type != NULL)
    free(net_type);

  return host_out;
}

esp_err_t mt_nvs_config_set_host_config(mt_nvs_host_t *host) {

  // check arg
  if (host == NULL) {
    ESP_LOGE(TAG, "%4d %s host NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (host->host == NULL) {
    ESP_LOGE(TAG, "%4d %s host->host NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (host->mqtt_port == NULL) {
    ESP_LOGE(TAG, "%4d %s host->mqtt_port NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (host->net_type == NULL) {
    ESP_LOGE(TAG, "%4d %s host->net_type NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (!(strcmp(host->net_type, "wifi") == 0 ||
        strcmp(host->net_type, "eth") == 0 ||
        strcmp(host->net_type, "4g") == 0)) {
    ESP_LOGE(TAG, "%4d %s host->net_type error:%s", __LINE__, __func__,
             host->net_type);
    return ESP_ERR_INVALID_ARG;
  }

  if (mt_nvs_write_string_config("host", host->host) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_write_string_config host error", __LINE__,
             __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (mt_nvs_write_string_config("mqtt_port", host->mqtt_port) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_write_string_config mqtt_port error", __LINE__,
             __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (mt_nvs_write_int32_config("http_port", host->http_port) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_write_string_config http_port error", __LINE__,
             __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (mt_nvs_write_string_config("net_type", host->net_type) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_write_string_config net_type error", __LINE__,
             __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  int32_t use_ssl = 0;
  if (host->use_ssl == true) {
    use_ssl = 1;
  }
  if (mt_nvs_write_int32_config("use_ssl", use_ssl) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_write_string_config use_ssl error", __LINE__,
             __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

esp_err_t mt_nvs_config_get_module_num(int *num_out) {
  if (mt_nvs_read_int32_config("mod_num", num_out) == false) {
    ESP_LOGE(TAG, "%4d %s get mod_num failed", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}

mt_nvs_module_t *mt_nvs_config_get_module(int index_in) {
  esp_err_t err = ESP_OK;
  mt_nvs_module_t *module_out = malloc(sizeof(mt_nvs_module_t));
  size_t size = 0;
  char key[32] = "";

  // check index_in
  if (index_in <= 0) {
    ESP_LOGE(TAG, "%4d %s index num error:%d", __LINE__, __func__, index_in);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  module_out->index = index_in;

  // get id
  sprintf(key, "mod_id_%d", index_in);
  module_out->id = mt_nvs_read_string_config(key, &size);
  if (module_out->id == NULL) {
    ESP_LOGE(TAG, "%4d %s get module %d id failed", __LINE__, __func__,
             index_in);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // get key
  sprintf(key, "mod_key_%d", index_in);
  module_out->key = mt_nvs_read_string_config(key, &size);
  if (module_out->key == NULL) {
    ESP_LOGE(TAG, "%4d %s get module %d key failed", __LINE__, __func__,
             index_in);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // get name
  sprintf(key, "mod_name_%d", index_in);
  module_out->name = mt_nvs_read_string_config(key, &size);
  if (module_out->name == NULL) {
    ESP_LOGE(TAG, "%4d %s get module %d name failed", __LINE__, __func__,
             index_in);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // get flow
  module_out->flows = malloc(sizeof(mt_nvs_flows_t));
  sprintf(key, "mod_%d_flow_num", index_in);
  if (mt_nvs_read_int32_config(key, &module_out->flows->flow_num) == false) {
    ESP_LOGE(TAG, "%4d %s get module %d flow num failed", __LINE__, __func__,
             index_in);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (module_out->flows->flow_num <= 0) {
    if (module_out->flows->flow_num < 0) {
      ESP_LOGE(TAG, "%4d %s error flow_num:%d", __LINE__, __func__,
               module_out->flows->flow_num);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  } else {
    module_out->flows->flows =
        malloc(sizeof(char) * module_out->flows->flow_num);
    for (int i = 0; i < module_out->flows->flow_num; i++) {
      sprintf(key, "mod_%d_flow_%d", index_in, i + 1);
      module_out->flows->flows[i] = mt_nvs_read_string_config(key, &size);
      if (module_out->flows->flows[i] == NULL) {
        ESP_LOGE(TAG, "%4d %s module:%d flow:%d get failed", __LINE__, __func__,
                 index_in, i);
        err = ESP_ERR_INVALID_ARG;
        goto EXIT;
      }
    }
  }

EXIT:
  if (err != ESP_OK) {
    mt_nvs_config_free_module(module_out);
    module_out = NULL;
  }

  return module_out;
}

esp_err_t mt_nvs_config_set_module(mt_nvs_module_t *module) {
  // check arg
  if (module == NULL) {
    ESP_LOGE(TAG, "%4d %s module NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (module->id == NULL) {
    ESP_LOGE(TAG, "%4d %s module->id NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (module->key == NULL) {
    ESP_LOGE(TAG, "%4d %s module->key NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (module->name == NULL) {
    ESP_LOGE(TAG, "%4d %s module->name NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (mt_nvs_write_string_config("mod_id_1", module->id) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_write_string_config mod_id_1 error", __LINE__,
             __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (mt_nvs_write_string_config("mod_key_1", module->key) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_write_string_config mod_key_1 error", __LINE__,
             __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (mt_nvs_write_string_config("mod_name_1", module->name) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_write_string_config mod_name_1 error",
             __LINE__, __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}

mt_nvs_flows_t *mt_nvs_config_get_flow(int index_in) {
  esp_err_t err = ESP_OK;
  mt_nvs_flows_t *flows_out = malloc(sizeof(mt_nvs_flows_t));
  size_t size = 0;
  char key[32] = "";

  // check index_in
  if (index_in <= 0) {
    ESP_LOGE(TAG, "%4d %s index num error:%d", __LINE__, __func__, index_in);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // get flow num
  sprintf(key, "mod_%d_flow_num", index_in);
  if (mt_nvs_read_int32_config(key, &flows_out->flow_num) == false) {
    ESP_LOGE(TAG, "%4d %s get mod:%d flow_num failed", __LINE__, __func__,
             index_in);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  flows_out->flows = malloc(flows_out->flow_num * sizeof(char));

  // get flows
  for (int i = 0; i < flows_out->flow_num; i++) {
    sprintf(key, "mod_%d_flow_%d", index_in, i + 1);
    flows_out->flows[i] = mt_nvs_read_string_config(key, &size);

    if (flows_out->flows[i] == NULL) {
      ESP_LOGE(TAG, "%4d %s module:%d flow:%d get failed", __LINE__, __func__,
               index_in, i);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

EXIT:
  if (err != ESP_OK) {
    mt_nvs_config_free_flows(flows_out);
    flows_out = NULL;
  }

  return flows_out;
}

char *mt_nvs_config_get_flow_name(int module_index, int flow_index) {
  char *flow_name = NULL;
  char key[32] = "";
  size_t size = 0;

  sprintf(key, "mod_%d_flow_%d", module_index, flow_index);
  flow_name = mt_nvs_read_string_config(key, &size);
  if (flow_name == NULL) {
    ESP_LOGE(TAG, "%4d %s module:%d flow:%d get flow_name failed", __LINE__,
             __func__, module_index, flow_index);
    return NULL;
  }

  return flow_name;
}

char *mt_nvs_config_get_net_type() {
  char *net_type = NULL;
  char key[32] = "net_type";
  size_t size = 0;

  net_type = mt_nvs_read_string_config(key, &size);
  if (net_type == NULL) {
    if (mt_nvs_write_string_config(key, "lan") == false) {
      ESP_LOGE(TAG, "%4d %s mt_nvs_write_string_config failed", __LINE__,
               __func__);
      return NULL;
    }
  }

  return net_type;
}

esp_err_t mt_nvs_config_get_flow_interval(int32_t index, int32_t *interval) {
  char key[32] = "";

  if (index <= 0) {
    ESP_LOGE(TAG, "%4d %s index:%d error", __LINE__, __func__, index);
    return ESP_ERR_INVALID_ARG;
  }

  sprintf(key, "mod_%d_intr", index);

  if (mt_nvs_read_int32_config(key, interval) == false) {
    ESP_LOGE(TAG, "%4d %s read mod:%d interval failed", __LINE__, __func__,
             index);
    if (mt_nvs_write_int32_config(key, DEFAULT_FLOW_INTERVAL) == false) {
      ESP_LOGE(TAG, "%4d %s write default mod:%d interval failed", __LINE__,
               __func__, index);
      return ESP_ERR_INVALID_RESPONSE;
    }
    *interval = DEFAULT_FLOW_INTERVAL;
  }

  return ESP_OK;
}

char *mt_nvs_config_get_json_config() {
  esp_err_t err = ESP_OK;
  cJSON *root = NULL;
  cJSON *module_json = NULL;
  char *buf = NULL;
  mt_nvs_host_t *host = NULL;
  mt_nvs_module_t *module = NULL;
  char temp_str[64] = "";

  // get host
  host = mt_nvs_config_get_host_config();
  if (host == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_get_host_config failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // get module 1
  module = mt_nvs_config_get_module(1);
  if (host == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_get_host_config failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // json marshal
  root = cJSON_CreateObject();
  cJSON_AddStringToObject(root, "host", host->host);
  cJSON_AddStringToObject(root, "http_port",
                          __itoa(host->http_port, temp_str, 10));
  cJSON_AddStringToObject(root, "mqtt_port", host->mqtt_port);
  cJSON_AddBoolToObject(root, "use_ssl", host->use_ssl);
  cJSON_AddStringToObject(root, "net_type", host->net_type);

  cJSON_AddItemToObject(root, "module", module_json = cJSON_CreateObject());
  cJSON_AddStringToObject(module_json, "id", module->id);
  int str_size = strlen(module->key);
  if (str_size < 10) {
    ESP_LOGE(TAG, "%4d %s key size:%d error", __LINE__, __func__, str_size);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }
  sprintf(temp_str, "%c%c%c***%c%c%c", module->key[0], module->key[1],
          module->key[2], module->key[str_size - 3], module->key[str_size - 2],
          module->key[str_size - 1]);
  cJSON_AddStringToObject(module_json, "key", temp_str);
  cJSON_AddStringToObject(module_json, "name", module->name);
  buf = cJSON_Print(root);

EXIT:
  mt_nvs_config_free_host(host);
  mt_nvs_config_free_module(module);
  cJSON_Delete(root);

  if (err != ESP_OK) {
    if (buf != NULL) {
      free(buf);
      buf = NULL;
    }
  }

  return buf;
}

esp_err_t mt_nvs_config_set_json_config(char *data) {
  esp_err_t err = ESP_OK;
  cJSON *root = NULL;
  cJSON *item = NULL;
  cJSON *module_item = NULL;
  mt_nvs_host_t *host = mt_nvs_config_new_host();
  mt_nvs_module_t *module = mt_nvs_config_new_module();

  // unmarshall
  root = cJSON_Parse(data);
  if (root == NULL) {
    ESP_LOGE(TAG, "%4d %s cJSON_Parse NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // get arg
  for (int i = 0; i < cJSON_GetArraySize(root); i++) {
    item = cJSON_GetArrayItem(root, i);
    if (cJSON_String == item->type) {
      if (strcmp(item->string, "host") == 0) {
        host->host = mt_utils_string_copy(item->valuestring);
      }
      if (strcmp(item->string, "http_port") == 0) {
        host->http_port = atoi(item->valuestring);
      }
      if (strcmp(item->string, "mqtt_port") == 0) {
        host->mqtt_port = mt_utils_string_copy(item->valuestring);
      }
      if (strcmp(item->string, "net_type") == 0) {
        host->net_type = mt_utils_string_copy(item->valuestring);
      }
    } else if (cJSON_True == item->type) {
      if (strcmp(item->string, "use_ssl") == 0) {
        host->use_ssl = true;
      }
    } else if (cJSON_False == item->type) {
      if (strcmp(item->string, "use_ssl") == 0) {
        host->use_ssl = false;
      }
    }
    if (cJSON_Object == item->type) {
      if (strcmp(item->string, "module") == 0) {
        for (int j = 0; j < cJSON_GetArraySize(item); j++) {
          module_item = cJSON_GetArrayItem(item, j);
          if (cJSON_String == module_item->type) {
            if (strcmp(module_item->string, "id") == 0) {
              module->id = mt_utils_string_copy(module_item->valuestring);
            }
            if (strcmp(module_item->string, "key") == 0) {
              module->key = mt_utils_string_copy(module_item->valuestring);
            }
            if (strcmp(module_item->string, "name") == 0) {
              module->name = mt_utils_string_copy(module_item->valuestring);
            }
          }
        }
      }
    }
  }

  // set config
  err = mt_nvs_config_set_host_config(host);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_set_host_config failed", __LINE__,
             __func__);
    goto EXIT;
  }
  err = mt_nvs_config_set_module(module);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_set_module failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  cJSON_Delete(root);
  mt_nvs_config_free_host(host);
  mt_nvs_config_free_module(module);

  return err;
}
