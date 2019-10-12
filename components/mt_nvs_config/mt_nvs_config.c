#include "mt_nvs_config.h"
#include "mt_nvs_storage.h"

// global config ==============================================================
static const char *TAG = "MT_NVS_CONFIG";

// global func ================================================================
esp_err_t mt_nvs_config_get_host_config(mt_nvs_host_t *host_out)
{
  size_t size = 0;

  // check host_out
  if (host_out == NULL)
  {
    ESP_LOGE(TAG, "%4d %s host_out NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  // get host_out->host
  if (host_out->host != NULL)
    free(host_out->host);
  host_out->host = mt_nvs_read_string_config("host", &size);
  if (host_out->host == NULL)
  {
    ESP_LOGE(TAG, "%4d %s get host failed", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  // get host_out->mqtt_port
  if (host_out->mqtt_port != NULL)
    free(host_out->mqtt_port);
  host_out->mqtt_port = mt_nvs_read_string_config("mqtt_port", &size);
  if (host_out->mqtt_port == NULL)
  {
    ESP_LOGE(TAG, "%4d %s get mqtt_port failed", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  // get host_out->http_port
  if (mt_nvs_read_int32_config("http_port", &host_out->http_port) == false)
  {
    ESP_LOGE(TAG, "%4d %s get http_port failed", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}

esp_err_t mt_nvs_config_get_module_num(int *num_out)
{
  if (mt_nvs_read_int32_config("mod_num", num_out) == false)
  {
    ESP_LOGE(TAG, "%4d %s get http_port failed", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}

esp_err_t mt_nvs_config_get_module(int index_in, mt_nvs_module_t *module_out)
{
  size_t size = 0;
  char key[16] = "";

  // check index_in
  if (index_in <= 0)
  {
    ESP_LOGE(TAG, "%4d %s index num error:%d", __LINE__, __func__, index_in);
    return ESP_ERR_INVALID_ARG;
  }

  // check module_out
  if (module_out == NULL)
  {
    ESP_LOGE(TAG, "%4d %s module_out NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  module_out->index = index_in;

  // get id
  if (module_out->id != NULL)
    free(module_out->id);
  sprintf(key, "mod_id_%d", index_in);
  module_out->id = mt_nvs_read_string_config(key, &size);
  if (module_out->id == NULL)
  {
    ESP_LOGE(TAG, "%4d %s get module %d id failed", __LINE__, __func__,
             index_in);
    return ESP_ERR_INVALID_ARG;
  }

  // get key
  if (module_out->key != NULL)
    free(module_out->key);
  sprintf(key, "mod_key_%d", index_in);
  module_out->key = mt_nvs_read_string_config(key, &size);
  if (module_out->key == NULL)
  {
    ESP_LOGE(TAG, "%4d %s get module %d key failed", __LINE__, __func__,
             index_in);
    return ESP_ERR_INVALID_ARG;
  }

  // get flow
  if (module_out->flows != NULL)
    free(module_out->flows);
  module_out->flows = malloc(sizeof(mt_nvs_flows_t));
  sprintf(key, "mod_%d_flow_num", index_in);
  if (mt_nvs_read_int32_config(key, &module_out->flows->flow_num) == false)
  {
    ESP_LOGE(TAG, "%4d %s get module %d flow num failed", __LINE__, __func__,
             index_in);
    return ESP_ERR_INVALID_ARG;
  }

  if (module_out->flows->flow_num <= 0)
  {
    if (module_out->flows->flow_num < 0)
    {
      ESP_LOGE(TAG, "%4d %s error flow_num:%d", __LINE__, __func__,
               module_out->flows->flow_num);
      return ESP_ERR_INVALID_ARG;
    }
  }
  else
  {
    module_out->flows->flows =
        malloc(sizeof(char) * module_out->flows->flow_num);
    for (int i = 0; i < module_out->flows->flow_num; i++)
    {
      sprintf(key, "mod_%d_flow_%d", index_in, i);
      module_out->flows->flows[i] = mt_nvs_read_string_config(key, &size);
      if (module_out->flows->flows[i] == NULL)
      {
        ESP_LOGE(TAG, "%4d %s module:%d flow:%d get failed", __LINE__, __func__,
                 index_in, i);
        return ESP_ERR_INVALID_ARG;
      }
    }
  }

  return ESP_OK;
}
