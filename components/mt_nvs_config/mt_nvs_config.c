#include "esp_log.h"
#include "string.h"

#include "mt_nvs_config.h"
#include "mt_nvs_storage.h"

// global config ==============================================================

static const char *TAG = "MT_NVS_CONFIG";

static const int DEFAULT_FLOW_INTERVAL = 600; // 10 minute

// help func ==================================================================

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
        strcmp(host->net_type, "eth") == 0)) {
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

  net_type = mt_nvs_read_string_config(key, &size);

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
