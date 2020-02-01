#include "esp_log.h"

#include "modbus_aew100_utils.h"

// global config ==============================================================

static const char *TAG = "modbus_aew100_utils";

// global func ================================================================

esp_err_t modbus_aew100_util_check_get_data_req(int32_t *addr,
                                                MtAew100__GetDataReq *req) {
  if (req == NULL) {
    ESP_LOGE(TAG, "%4d %s req NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (req->addr <= 0) {
    ESP_LOGE(TAG, "%4d %s req->addr:%d check error", __LINE__, __func__,
             req->addr);
    return ESP_ERR_INVALID_ARG;
  }

  *addr = req->addr;

  return ESP_OK;
}

esp_err_t
modbus_aew100_util_check_get_current_req(int32_t *addr,
                                         MtAew100__GetCurrentReq *req) {
  if (req == NULL) {
    ESP_LOGE(TAG, "%4d %s req NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (req->addr <= 0) {
    ESP_LOGE(TAG, "%4d %s req->addr:%d check error", __LINE__, __func__,
             req->addr);
    return ESP_ERR_INVALID_ARG;
  }

  *addr = req->addr;

  return ESP_OK;
}
