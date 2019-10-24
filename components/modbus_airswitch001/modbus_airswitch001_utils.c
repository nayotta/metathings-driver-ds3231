#include "esp_log.h"

#include "modbus_airswitch001_utils.h"

// global config ==============================================================
static const char *TAG = "modbus_airswitch001_utils";

// global func ================================================================
esp_err_t modbus_airswitch001_util_copy_datas(airswitch_data_t *datas_in,
                                              Data *datas_out)
{
  if (datas_in == NULL)
  {
    ESP_LOGE(TAG, "%4d %s datas_in NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (datas_out == NULL)
  {
    ESP_LOGE(TAG, "%4d %s datas_out NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  datas_out->state = datas_in->state;
  datas_out->ctrl = datas_in->ctrl;
  datas_out->votage = datas_in->votage;
  datas_out->leakcurrent = datas_in->leak_current;
  datas_out->power = datas_in->power;
  datas_out->temp = datas_in->temp;
  datas_out->current = datas_in->current;

  return ESP_OK;
}

esp_err_t modbus_airswitch001_util_copy_configs(airswitch_config_t *config_in,
                                                Config *config_out)
{
  if (config_in == NULL)
  {
    ESP_LOGE(TAG, "%4d %s datas_in NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (config_out == NULL)
  {
    ESP_LOGE(TAG, "%4d %s datas_out NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  config_out->votagehigh = config_in->votage_high;
  config_out->votagelow = config_in->votage_low;
  config_out->leakcurrenthigh = config_in->leak_current_high;
  config_out->powerhigh = config_in->power_high;
  config_out->temphigh = config_in->temp_high;
  config_out->currenthigh = config_in->current_high;

  return ESP_OK;
}

esp_err_t modbus_airswitch001_util_check_get_req(int32_t *addr, GetReq *req)
{
  if (req == NULL)
  {
    ESP_LOGE(TAG, "%4d %s req NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (req->addr <= 0)
  {
    ESP_LOGE(TAG, "%4d %s req->addr:%d check error", __LINE__, __func__,
             req->addr);
    return ESP_ERR_INVALID_ARG;
  }

  *addr = req->addr;

  return ESP_OK;
}

esp_err_t modbus_airswitch001_util_check_set_state_req(int32_t *addr,
                                                       SetStateReq *req)
{
  if (req == NULL)
  {
    ESP_LOGE(TAG, "%4d %s req NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (req->addr <= 0)
  {
    ESP_LOGE(TAG, "%4d %s req->addr:%d check error", __LINE__, __func__,
             req->addr);
    return ESP_ERR_INVALID_ARG;
  }

  *addr = req->addr;

  if (req->state == NULL)
  {
    ESP_LOGE(TAG, "%4d %s req->state  NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (req->state->data == NULL)
  {
    ESP_LOGE(TAG, "%4d %s req->state->data  NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}

esp_err_t modbus_airswitch001_util_check_set_config_req(int32_t *addr,
                                                        SetConfigReq *req)
{
  if (req == NULL)
  {
    ESP_LOGE(TAG, "%4d %s req NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (req->addr <= 0)
  {
    ESP_LOGE(TAG, "%4d %s req->addr:%d check error", __LINE__, __func__,
             req->addr);
    return ESP_ERR_INVALID_ARG;
  }

  *addr = req->addr;

  if (req->config == NULL)
  {
    ESP_LOGE(TAG, "%4d %s req->config  NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}

esp_err_t modbus_airswitch001_util_check_set_quality_req(int32_t *addr,
                                                         SetQualityReq *req)
{
  if (req == NULL)
  {
    ESP_LOGE(TAG, "%4d %s req NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (req->addr <= 0)
  {
    ESP_LOGE(TAG, "%4d %s req->addr:%d check error", __LINE__, __func__,
             req->addr);
    return ESP_ERR_INVALID_ARG;
  }

  *addr = req->addr;

  if (req->quality == NULL)
  {
    ESP_LOGE(TAG, "%4d %s req->quality  NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (req->quality->quality == NULL)
  {
    ESP_LOGE(TAG, "%4d %s req->quality->quality  NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}

esp_err_t modbus_airswitch001_util_check_set_leak_test_req(
    int32_t *addr, SetLeakTestReq *req)
{
  if (req == NULL)
  {
    ESP_LOGE(TAG, "%4d %s req NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (req->addr <= 0)
  {
    ESP_LOGE(TAG, "%4d %s req->addr:%d check error", __LINE__, __func__,
             req->addr);
    return ESP_ERR_INVALID_ARG;
  }

  *addr = req->addr;

  return ESP_OK;
}