#include "esp_err.h"
#include "esp_log.h"

#include "mt_nvs_config.h"
#include "mt_nvs_storage.h"

#include "rs232_lora_ebyte_module_manage.h"

// static define ==============================================================

static const char *TAG = "RS232_LORA_EBYTE_MODULE_MANAGE";
static rs232_lora_ebyte_module_manage_t *MODULE_MANAGE = NULL;

// global func ================================================================

esp_err_t
rs232_lora_ebyte_module_manage_add(rs232_lora_ebyte_module_t *module) {
  rs232_lora_ebyte_module_manage_t *temp_manage = NULL;

  if (MODULE_MANAGE == NULL) {
    MODULE_MANAGE = malloc(sizeof(rs232_lora_ebyte_module_manage_t));
    MODULE_MANAGE->module_size = 1;
    MODULE_MANAGE->modules = malloc(MODULE_MANAGE->module_size *
                                    sizeof(rs232_lora_ebyte_module_t *));
    MODULE_MANAGE->modules[0] = module;
    return ESP_OK;
  } else {
    temp_manage = MODULE_MANAGE;
    MODULE_MANAGE = malloc(sizeof(rs232_lora_ebyte_module_manage_t));
    MODULE_MANAGE->module_size = temp_manage->module_size + 1;
    MODULE_MANAGE->modules = malloc(MODULE_MANAGE->module_size *
                                    sizeof(rs232_lora_ebyte_module_t *));
    for (int i = 0; i < MODULE_MANAGE->module_size - 1; i++) {
      MODULE_MANAGE->modules[i] = temp_manage->modules[i];
    }
    MODULE_MANAGE->modules[MODULE_MANAGE->module_size - 1] = module;
    return ESP_OK;
  }
}

esp_err_t
rs232_lora_ebyte_module_manage_get_id_by_module_index(int module_index,
                                                      int16_t *id_out) {
  int match = 0;

  if (MODULE_MANAGE == NULL) {
    ESP_LOGE(TAG, "%4d %s MODULE_MANAGE NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  for (int i = 0; i < MODULE_MANAGE->module_size; i++) {
    if (MODULE_MANAGE->modules[i]->module_index == module_index) {
      *id_out = MODULE_MANAGE->modules[i]->id;
      match++;
    }
  }

  if (match == 0) {
    ESP_LOGE(TAG, "%4d %s module_index:%d no match", __LINE__, __func__,
             module_index);
    return ESP_ERR_INVALID_RESPONSE;
  } else if (match > 1) {
    ESP_LOGW(TAG, "%4d %s module_index:%d match too much times", __LINE__,
             __func__, module_index);
  }

  return ESP_OK;
}

esp_err_t rs232_lora_ebyte_module_manage_init() {
  esp_err_t err = ESP_OK;
  int lora_num = 0;
  char key[32] = "";

  if (mt_nvs_read_int32_config("lora_num", &lora_num) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_read_int32_config lora_num failed", __LINE__,
             __func__);
    return ESP_ERR_INVALID_ARG;
  }

  ESP_LOGI(TAG, "%4d %s lora_num:%d", __LINE__, __func__, lora_num);

  for (int i = 1; i <= lora_num; i++) {
    int lora_addr = 0;

    sprintf(key, "lora_addr_%d", i);
    if (mt_nvs_read_int32_config(key, &lora_addr) == false) {
      ESP_LOGE(TAG, "%4d %s mt_nvs_read_int32_config %s failed", __LINE__,
               __func__, key);
      return ESP_ERR_INVALID_ARG;
    }
    rs232_lora_ebyte_module_t *module =
        malloc(sizeof(rs232_lora_ebyte_module_t));
    module->id = lora_addr;
    module->module_index = i;
    err = rs232_lora_ebyte_module_manage_add(module);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_module_manage_add failed",
               __LINE__, __func__);
      return ESP_ERR_INVALID_RESPONSE;
    }

    ESP_LOGI(TAG, "%4d %s add index:%d addr:%d", __LINE__, __func__, i,
             lora_addr);
  }

  return ESP_OK;
}
