#include "huawei_mqtt_utils.h"

#include "math.h"

#include "mt_utils_string.h"

// static define ==============================================================

static const char *TAG = "HUAWEI_MQTT_UTILS";

// help func ==================================================================

huawei_cmd_t *huawei_mqtt_utils_new_cmd_t() {
  huawei_cmd_t *cmd_t = malloc(sizeof(huawei_cmd_t));

  cmd_t->msg_type = NULL;
  cmd_t->mid = 0;
  cmd_t->method = NULL;
  cmd_t->paras = NULL;
  cmd_t->serviceID = NULL;
  cmd_t->clientID = NULL;

  return cmd_t;
}

void huawei_mqtt_utils_free_cmd_t(huawei_cmd_t *cmd_t) {
  if (cmd_t == NULL)
    return;

  if (cmd_t->msg_type != NULL)
    free(cmd_t->msg_type);

  if (cmd_t->method != NULL)
    free(cmd_t->method);

  if (cmd_t->paras != NULL)
    cJSON_Delete(cmd_t->paras);

  if (cmd_t->serviceID != NULL)
    free(cmd_t->serviceID);

  if (cmd_t->clientID != NULL)
    free(cmd_t->clientID);

  free(cmd_t);
}

// global func ================================================================

char *huawei_mqtt_utils_gen_push(cJSON *data, char *clientID, char *serviceID) {
  char *buf = NULL;
  cJSON *devices_array = NULL;
  cJSON *devices_item = cJSON_CreateObject();
  cJSON *services_array = NULL;
  cJSON *services_item = cJSON_CreateObject();
  cJSON *root = cJSON_CreateObject();

  devices_array = cJSON_AddArrayToObject(root, "devices");
  cJSON_AddItemToArray(devices_array, devices_item);
  cJSON_AddStringToObject(devices_item, "deviceId", clientID);
  services_array = cJSON_AddArrayToObject(devices_item, "services");
  cJSON_AddItemToArray(services_array, services_item);
  cJSON_AddItemToObject(services_item, "data", cJSON_Duplicate(data, true));
  cJSON_AddStringToObject(services_item, "eventTime", "20201204T080000Z");
  cJSON_AddStringToObject(services_item, "serviceId", serviceID);

  buf = cJSON_Print(root);

  ESP_LOGI(TAG, "%4d %s push data:%s", __LINE__, __func__, buf);

  cJSON_Delete(root);
  return buf;
}

char *huawei_mqtt_utils_gen_resp(cJSON *res_para, char *state, int errcode,
                                 int mid) {
  char *buf = NULL;
  cJSON *body = NULL;
  cJSON *root = cJSON_CreateObject();

  cJSON_AddNumberToObject(root, "errcode", errcode);
  cJSON_AddNumberToObject(root, "mid", mid);
  cJSON_AddStringToObject(root, "msgType", "deviceRsp");

  cJSON_AddItemToObject(root, "body", body = cJSON_CreateObject());
  cJSON_AddStringToObject(body, "state", state);
  cJSON_AddItemToObject(body, "orginParameters",
                        cJSON_Duplicate(res_para, true));

  buf = cJSON_Print(root);

  ESP_LOGI(TAG, "%4d %s res_data:%s", __LINE__, __func__, buf);

  cJSON_Delete(root);
  return buf;
}

huawei_cmd_t *huawei_mqtt_utils_parse_cmd(char *msg, int size) {
  esp_err_t err = ESP_OK;
  cJSON *root = NULL;
  cJSON *item = NULL;
  huawei_cmd_t *cmd_t = huawei_mqtt_utils_new_cmd_t();
  char *buf = NULL;

  if (size > 0) {
    buf = malloc(size + 1);
    memcpy(buf, msg, size);
    buf[size] = '\0';
  }
  ESP_LOGI(TAG, "%4d %s req:%s", __LINE__, __func__, buf);

  root = cJSON_Parse(buf);
  if (root == NULL) {
    ESP_LOGE(TAG, "%4d %s cJSON_Parse msg NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  for (int i = 0; i < cJSON_GetArraySize(root); i++) {
    item = cJSON_GetArrayItem(root, i);
    if (item->type == cJSON_String) {
      if (strcmp(item->string, "msgType") == 0) {
        cmd_t->msg_type = mt_utils_string_copy(item->valuestring);
      }

      if (strcmp(item->string, "cmd") == 0) {
        cmd_t->method = mt_utils_string_copy(item->valuestring);
      }

      if (strcmp(item->string, "serviceId") == 0) {
        cmd_t->serviceID = mt_utils_string_copy(item->valuestring);
      }

      if (strcmp(item->string, "deviceId") == 0) {
        cmd_t->clientID = mt_utils_string_copy(item->valuestring);
      }

    } else if (item->type == cJSON_Number) {
      if (strcmp(item->string, "mid") == 0)
        cmd_t->mid = (int)item->valuedouble;
    } else if (item->type == cJSON_Object) {
      if (strcmp(item->string, "paras") == 0) {
        cmd_t->paras = cJSON_Duplicate(item, true);
      }
    }
  }

  if (cmd_t->msg_type == NULL) {
    ESP_LOGE(TAG, "%4d %s msg_type NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  } else {
    if (strcmp(cmd_t->msg_type, "cloudReq") != 0) {
      ESP_LOGE(TAG, "%4d %s except:%s get:%s", __LINE__, __func__, "cloudReq",
               cmd_t->msg_type);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  if (cmd_t->method == NULL) {
    ESP_LOGE(TAG, "%4d %s method NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (cmd_t->serviceID == NULL) {
    ESP_LOGE(TAG, "%4d %s serviceID NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (cmd_t->clientID == NULL) {
    ESP_LOGE(TAG, "%4d %s clientID NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

EXIT:
  if (err != ESP_OK) {
    huawei_mqtt_utils_free_cmd_t(cmd_t);
    cmd_t = NULL;
  }
  cJSON_Delete(root);
  if (buf != NULL)
    free(buf);
  return cmd_t;
}
