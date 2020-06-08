#include "modbus_aew100_utils.h"

#include "mt_nvs_storage.h"
#include "mt_utils_error.h"

// global config ==============================================================

static const char *TAG = "modbus_aew100_utils";

// global func ================================================================

esp_err_t modbus_aew100_utils_check_get_data_req(int32_t *addr,
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
modbus_aew100_utils_check_get_current_req(int32_t *virt_addr,
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

  *virt_addr = req->addr;

  return ESP_OK;
}

esp_err_t modbus_aew100_utils_convert_virt_addr_to_real_addr(
    int32_t module_index, int32_t virt_addr, int32_t *real_addr) {
  char key[24] = "";

  sprintf(key, "mod_%d_f_%d_addr", module_index, virt_addr);

  if (mt_nvs_read_int32_config(key, real_addr) == false) {
    ESP_LOGE(TAG, "%4d %s key:\"%s\" not exist", __LINE__, __func__, key);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}

void modbus_aew100_get_current_res_init(MtAew100__GetCurrentRes *res) {
  res->has_rescode = true;
  res->rescode = MT_ERR_NO_ERR;
  res->currentdata = NULL;
}

void modbus_aew100_current_data_init(MtAew100__CurrentData *data) {
  data->has_currenta = true;
  data->has_currentb = true;
  data->has_currentc = true;
}

void modbus_aew100_get_data_res_init(MtAew100__GetDataRes *res) {
  res->has_code = true;
  res->code = MT_ERR_NO_ERR;
  res->data = NULL;
}

void modbus_aew100_data_init(MtAew100__Data *data) {
  data->has_currenta = true;
  data->has_currentb = true;
  data->has_currentc = true;

  data->has_votagea = true;
  data->has_votageb = true;
  data->has_currentc = true;

  data->has_activepowera = true;
  data->has_activepowerb = true;
  data->has_activepowerc = true;

  data->has_reactivepowera = true;
  data->has_reactivepowerb = true;
  data->has_reactivepowerc = true;

  data->has_powerfactora = true;
  data->has_powerfactorb = true;
  data->has_powerfactorc = true;

  data->has_qualitya = true;
  data->has_qualityb = true;
  data->has_qualityc = true;

  data->has_tempa = true;
  data->has_tempb = true;
  data->has_tempc = true;

  data->has_maxpowera = true;
  data->has_maxpowerb = true;
  data->has_maxpowerc = true;
}
