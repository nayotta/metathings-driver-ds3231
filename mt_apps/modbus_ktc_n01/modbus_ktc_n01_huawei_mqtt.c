
#include "modbus_ktc_n01_huawei_mqtt.h"
#include "cJSON.h"
#include "esp_err.h"
#include "esp_log.h"

#include "huawei_mqtt.h"
#include "huawei_mqtt_manage.h"
#include "huawei_mqtt_utils.h"

#include "modbus_ktc_n01.h"

// global config ==============================================================

static const char *TAG = "HUAWEI_MQTT";

// static func ================================================================

static void huawei_mqtt_process_set_cmd(char *msg, int size) {
  esp_err_t err = ESP_OK;
  huawei_cmd_t *cmd_t = NULL;
  cJSON *item = NULL;
  cJSON *res = cJSON_CreateObject();
  int32_t port = 0;
  int32_t cmd = 0;
  double code = 0;
  char *res_buf = NULL;

  // parse msg
  cmd_t = huawei_mqtt_utils_parse_cmd(msg, size);
  if (cmd_t == NULL) {
    ESP_LOGE(TAG, "%4d %s huawei_mqtt_utils_parse_cmd null", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // parse paras
  for (int i = 0; i < cJSON_GetArraySize(cmd_t->paras); i++) {
    item = cJSON_GetArrayItem(cmd_t->paras, i);
    if (item->type == cJSON_Number) {
      if (strcmp(item->string, "port") == 0) {
        port = (int)item->valuedouble;
      }

      if (strcmp(item->string, "cmd") == 0) {
        cmd = (int)item->valuedouble;
      }
    }
  }
  ESP_LOGI(TAG, "%4d %s req port:%d cmd:%d", __LINE__, __func__, port, cmd);

  // cmd process
  err = modbus_ktc_n01_set_cmd(port, cmd);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_ktc_n01_set_cmd failed", __LINE__, __func__);
    code = 2;
    goto ERROR;
  }

  goto ERROR;

ERROR:
  cJSON_AddNumberToObject(res, "code", code);
  cJSON_AddStringToObject(res, "content", "");

  // gen response
  res_buf = huawei_mqtt_utils_gen_resp(res, "ok", 0, cmd_t->mid);
  if (res_buf == NULL) {
    ESP_LOGE(TAG, "%4d %s huawei_mqtt_utils_gen_resp NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // sent response
  err = huawei_mqtt_pub_msg("/commandResponse", res_buf, strlen(res_buf));
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s huawei_mqtt_pub_msg failed", __LINE__, __func__);
  } else {
    ESP_LOGI(TAG, "%4d %s huawei_mqtt_pub_msg success", __LINE__, __func__);
  }

EXIT:
  huawei_mqtt_utils_free_cmd_t(cmd_t);
  cJSON_Delete(res);
  if (res_buf != NULL)
    free(res_buf);
  return;
}

// global func  ===============================================================

void modbus_ktc_n01_huawei_mqtt_init() {
  huawei_mqtt_manage_add_handle(huawei_mqtt_process_set_cmd, "cmd", "SetCmd");
}
