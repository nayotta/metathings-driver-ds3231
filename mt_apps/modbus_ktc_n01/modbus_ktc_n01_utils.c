#include "esp_log.h"

#include "modbus_ktc_n01_utils.h"

// global config ==============================================================

static const char *TAG = "modbus_ktc_n01_utils";

// global func ================================================================

esp_err_t modbus_ktc_n01_util_check_set_cmd_req(MtKtcN01__SetCmdReq *req) {
  if (req == NULL) {
    ESP_LOGE(TAG, "%4d %s req NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (req->port <= 0) {
    ESP_LOGE(TAG, "%4d %s req->port:%d check error", __LINE__, __func__,
             req->port);
    return ESP_ERR_INVALID_ARG;
  }

  if (req->cmd <= 0) {
    ESP_LOGE(TAG, "%4d %s req->cmd:%d check error", __LINE__, __func__,
             req->cmd);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}
