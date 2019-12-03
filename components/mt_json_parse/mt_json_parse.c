#include "cJSON.h"
#include "stdio.h"

#include "esp_log.h"

#include "mt_json_parse.h"

// global define ==============================================================
static const char *TAG = "MT_JSON_PARSE";

// global func ================================================================
uint8_t *mt_json_parse(char *buf_in) {
  cJSON *root = NULL;
  cJSON *item = NULL;

  root = cJSON_Parse(buf_in);
  if (root == NULL) {
    ESP_LOGE(TAG, "%4d %s cJSON_Parse error", __LINE__, __func__);
    goto ERROR;
  }

  for (int i = 0; i < cJSON_GetArraySize(root); i++) {
    item = cJSON_GetArrayItem(root, i);

    if (item->type == 0) {
    }
  }

ERROR:
  return NULL;
}
