#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "mt_module_http_manage.h"
#include "mt_module_mqtt.h"
#include "mt_mqtt_lan.h"
#include "mt_mqtt_utils.h"

// global define ==============================================================

static const char *TAG = "MT_MODULE_HTTP_MANAGE";
static mt_module_http_manage_t *MODULE_MANAGE = NULL;

// global func ================================================================

esp_err_t mt_module_http_manage_add(mt_module_http_t *module_http,
                                    int module_index) {
  mt_module_http_manage_t *temp_manage = NULL;

  if (MODULE_MANAGE == NULL) {
    MODULE_MANAGE = malloc(sizeof(mt_module_http_manage_t));
    MODULE_MANAGE->module_size = 0;
  }

  temp_manage = MODULE_MANAGE;
  MODULE_MANAGE = malloc(sizeof(mt_module_http_manage_t));
  MODULE_MANAGE->module_size = temp_manage->module_size + 1;
  ESP_LOGI(TAG, "%4d %s new module size=%d", __LINE__, __func__,
           MODULE_MANAGE->module_size);

  MODULE_MANAGE->moudle_https = (mt_module_http_t **)malloc(
      MODULE_MANAGE->module_size * sizeof(mt_module_http_t *));
  for (int i = 0; i < MODULE_MANAGE->module_size - 1; i++) {
    MODULE_MANAGE->moudle_https[i] = temp_manage->moudle_https[i];
  }
  MODULE_MANAGE->module_index =
      malloc(MODULE_MANAGE->module_size * sizeof(int));
  for (int i = 0; i < MODULE_MANAGE->module_size - 1; i++) {
    MODULE_MANAGE->module_index[i] = temp_manage->module_index[i];
  }

  MODULE_MANAGE->moudle_https[MODULE_MANAGE->module_size - 1] = module_http;
  MODULE_MANAGE->module_index[MODULE_MANAGE->module_size - 1] = module_index;

  if (temp_manage != NULL)
    free(temp_manage);
  return ESP_OK;
}

esp_err_t mt_module_http_manage_get_index_by_module_id(char *module_id,
                                                       int *module_index) {
  int match = 0;

  if (module_id == NULL) {
    ESP_LOGE(TAG, "%4d %s module_id NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (MODULE_MANAGE == NULL) {
    ESP_LOGE(TAG, "%4d %s MODULE_MANAGE NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  for (int i = 0; i < MODULE_MANAGE->module_size; i++) {
    if (MODULE_MANAGE->moudle_https[i] == NULL) {
      continue;
    } else {
      if (MODULE_MANAGE->moudle_https[i]->module == NULL) {
        continue;
      } else {
        if (MODULE_MANAGE->moudle_https[i]->module->id == NULL) {
          continue;
        } else {
          if (strcmp(MODULE_MANAGE->moudle_https[i]->module->id, module_id) ==
              0) {
            *module_index = MODULE_MANAGE->module_index[i];
            match++;
            break;
          }
        }
      }
    }
  }

  if (match == 0) {
    ESP_LOGE(TAG, "%4d %s module_id no match", __LINE__, __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return ESP_OK;
}
