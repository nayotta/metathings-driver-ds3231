#include "esp_err.h"
#include "esp_log.h"

#include "rs232_lora_ebyte_module_manage.h"

// static define ==============================================================

static const char *TAG = "RS232_LORA_EBYTE_UNARYCALL_MANAGE";
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
