#include "huawei_mqtt_manage.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "cJSON.h"
#include "esp_err.h"
#include "esp_log.h"
#include "string.h"

#include "mt_utils_string.h"

// global define ==============================================================

static const char *TAG = "HUAWEI_MQTT_MANAGE";

static huawei_mqtt_manage_t *app_handle = NULL;

// static func ================================================================

huawei_mqtt_msg_t *huawei_mqtt_manage_new_msg() {
  huawei_mqtt_msg_t *msg = malloc(sizeof(huawei_mqtt_msg_t));

  msg->buf = NULL;
  msg->topic = NULL;
  msg->buf_size = 0;

  return msg;
}

void huawei_mqtt_manage_free_msg(huawei_mqtt_msg_t *msg) {
  if (msg == NULL)
    return;

  if (msg->buf != NULL)
    free(msg->buf);

  if (msg->topic != NULL)
    free(msg->topic);

  free(msg);
}

static void huawei_mqtt_manage_handle_loop(huawei_mqtt_msg_t *msg) {
  cJSON *root = NULL, *item = NULL;
  char *method = NULL;
  char *serviceId = NULL;

  root = cJSON_Parse((char *)msg->buf);
  if (root == NULL) {
    ESP_LOGE(TAG, "%4d %s cJSON_Parse NULL", __LINE__, __func__);
    goto EXIT;
  }

  for (int i = 0; i < cJSON_GetArraySize(root); i++) {
    item = cJSON_GetArrayItem(root, i);
    if (strcmp(item->string, "cmd") == 0) {
      method = item->valuestring;
    }

    if (strcmp(item->string, "serviceId") == 0) {
      serviceId = item->valuestring;
    }
  }

  // app api
  if (app_handle == NULL) {
    ESP_LOGW(TAG, "%4d %s no app handle", __LINE__, __func__);
  } else {
    for (int i = 0; i < app_handle->handle_size; i++) {
      if (strcmp(serviceId, app_handle->serviceID[i]) == 0 &&
          strcmp(method, app_handle->methods[i]) == 0) {
        ESP_LOGI(TAG, "%4d %s match serviceId:%s method:%s", __LINE__, __func__,
                 serviceId, method);
        (*(app_handle->handles[i]))((char *)msg->buf, msg->buf_size);
        goto EXIT;
      }
    }
  }

  ESP_LOGE(TAG, "%4d %s get unexcept serviceId:%s method:%s", __LINE__,
           __func__, serviceId, method);

EXIT:
  cJSON_Delete(root);
  huawei_mqtt_manage_free_msg(msg);
  vTaskDelete(NULL);
}

// global func ================================================================

void huawei_mqtt_manage_add_handle(void (*handle)(char *msg, int size),
                                   char *serviceID, char *method) {
  ESP_LOGI(TAG, "%4d %s add handle service:%s method:\"%s\"", __LINE__,
           __func__, serviceID, method);

  if (app_handle == NULL) {
    app_handle = malloc(sizeof(huawei_mqtt_manage_t));
    app_handle->handle_size = 1;
    app_handle->handles = malloc(sizeof(huawei_mqtt_manage_handle_t));
    app_handle->handles[0] = handle;
    app_handle->methods = malloc(sizeof(char *));
    app_handle->methods[0] = malloc(strlen(method) + 1);
    memcpy(app_handle->methods[0], method, strlen(method) + 1);
    app_handle->serviceID = malloc(sizeof(char *));
    app_handle->serviceID[0] = malloc(strlen(serviceID) + 1);
    memcpy(app_handle->serviceID[0], serviceID, strlen(serviceID) + 1);

  } else {
    huawei_mqtt_manage_t *temp_handle = app_handle;
    app_handle = malloc(sizeof(huawei_mqtt_manage_t));
    app_handle->handle_size = temp_handle->handle_size + 1;
    app_handle->handles =
        malloc(sizeof(huawei_mqtt_manage_handle_t) * app_handle->handle_size);
    app_handle->methods = malloc(sizeof(char *) * app_handle->handle_size);
    app_handle->serviceID = malloc(sizeof(char *) * app_handle->handle_size);
    for (int i = 0; i < app_handle->handle_size - 1; i++) {
      app_handle->handles[i] = temp_handle->handles[i];
      app_handle->methods[i] = temp_handle->methods[i];
      app_handle->serviceID[i] = temp_handle->serviceID[i];
    }
    app_handle->handles[app_handle->handle_size - 1] = handle;
    app_handle->methods[app_handle->handle_size - 1] =
        malloc(strlen(method) + 1);
    memcpy(app_handle->methods[app_handle->handle_size - 1], method,
           strlen(method) + 1);
    app_handle->serviceID[app_handle->handle_size - 1] =
        malloc(strlen(serviceID) + 1);
    memcpy(app_handle->serviceID[app_handle->handle_size - 1], serviceID,
           strlen(serviceID) + 1);

    free(temp_handle->handles);
    free(temp_handle->methods);
    free(temp_handle->serviceID);
    free(temp_handle);
  }
}

void huawei_mqtt_manage_handle_process(char *topic, void *buf, int size) {
  huawei_mqtt_msg_t *msg = huawei_mqtt_manage_new_msg();

  msg->topic = mt_utils_string_copy(topic);
  msg->buf_size = size;
  msg->buf = malloc(size);
  memcpy(msg->buf, buf, size);

  xTaskCreate((TaskFunction_t)huawei_mqtt_manage_handle_loop,
              "HUAWEI_MQTT_TASK", 4 * 1024, msg, 10, NULL);
}
