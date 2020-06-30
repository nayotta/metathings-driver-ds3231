#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "mt_module_flow.h"
#include "mt_module_flow_manage.h"
#include "mt_module_mqtt.h"
#include "mt_mqtt_lan.h"
#include "mt_mqtt_utils.h"

// global define ==============================================================

static const char *TAG = "MT_MODULE_FLOW_MANAGE";
static mt_module_flow_manage_t *FLOW_MANAGE = NULL;

// global func ================================================================

esp_err_t mt_module_flow_manage_mqtt_process(char *topic, uint8_t *buf,
                                             int size) {
  esp_err_t err = ESP_OK;
  char *session_string = NULL;
  bool match = false;

  // check FLOW_MANAGER
  if (FLOW_MANAGE == NULL) {
    ESP_LOGE(TAG, "%4d %s FLOW_MANAGER is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  } else {
    if (FLOW_MANAGE->flows_size == 0) {
      ESP_LOGE(TAG, "%4d %s FLOW_MANAGER->flows_size is zero", __LINE__,
               __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }

    if (FLOW_MANAGE->flows == NULL) {
      ESP_LOGE(TAG, "%4d %s FLOW_MANAGER->flows is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  // get session_id
  session_string = mt_mqtt_utils_get_session_string_from_topic(topic);
  if (session_string == NULL) {
    ESP_LOGE(TAG,
             "%4d %s mt_mqtt_utils_get_session_id_from_topic failed, topic:%s",
             __LINE__, __func__, topic);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // match session
  for (int i = 0; i < FLOW_MANAGE->flows_size; i++) {
    if (FLOW_MANAGE->flows[i] == NULL) {
      ESP_LOGE(TAG, "%4d %s FLOW_MANAGER->flows[%d] is NULL", __LINE__,
               __func__, i);
      continue;
    }

    if (FLOW_MANAGE->flows[i]->session == NULL) {
      ESP_LOGE(TAG, "%4d %s FLOW_MANAGER->flows[%d]->session is NULL", __LINE__,
               __func__, i);
      continue;
    }

    if (strcmp(FLOW_MANAGE->flows[i]->session, session_string) == 0) {
      match = true;
      mt_module_flow_process(FLOW_MANAGE->flows[i], topic, buf, size);
    }
  }

  if (match != true) {
    ESP_LOGE(TAG, "%4d %s unexcept session_id:%s", __LINE__, __func__,
             session_string);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

EXIT:
  if (session_string != NULL) {
    free(session_string);
  }
  return err;
}

static void mt_module_flow_manage_push_task_loop(mt_module_flow_t *flow) {
  mt_module_mqtt_msg_t *mqtt_msg = NULL;
  QueueHandle_t *handle = NULL;
  char *flow_name = NULL;
  esp_err_t err = ESP_OK;
  BaseType_t ret = pdFALSE;

  for (int i = 0; i < FLOW_MANAGE->flows_size; i++) {
    if (FLOW_MANAGE->flows[i] == flow) {
      // ESP_LOGW(TAG, "%4d %s flow:%p target:%p", __LINE__, __func__,
      //        FLOW_MANAGE->flows[i], flow);
      handle = FLOW_MANAGE->flows_handle[i];
      flow_name = FLOW_MANAGE->flows[i]->flow->name;
      flow = FLOW_MANAGE->flows[i];
      break;
    }
  }

  if (handle == NULL) {
    ESP_LOGE(TAG, "%4d %s handle NULL", __LINE__, __func__);
    vTaskDelete(NULL);
  } else {
    ESP_LOGI(TAG, "%4d %s handle:%p loop", __LINE__, __func__, handle);
  }

  // wait push flow msg
  while (1) {
    ret = xQueueReceive(handle, &mqtt_msg, portMAX_DELAY);
    if (ret != pdTRUE) {
      continue;
    }
    err = mqtt_pub_msg(mqtt_msg->topic, mqtt_msg->buf, mqtt_msg->buf_size);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s flow:%s mqtt_pub_msg failed", __LINE__, __func__,
               flow_name);
    }

    ESP_LOGI(TAG, "%4d %s flow:%s push flow success", __LINE__, __func__,
             flow_name);
  }
}

esp_err_t mt_module_flow_manage_add(mt_module_flow_t *flow) {
  mt_module_flow_manage_t *temp_manage = NULL;

  if (FLOW_MANAGE == NULL) {
    FLOW_MANAGE = malloc(sizeof(mt_module_flow_manage_t));
    FLOW_MANAGE->flows_size = 0;
  }

  temp_manage = FLOW_MANAGE;
  FLOW_MANAGE =
      malloc((temp_manage->flows_size + 1) * sizeof(mt_module_flow_manage_t *));
  FLOW_MANAGE->flows_size = temp_manage->flows_size + 1;
  ESP_LOGI(TAG, "%4d %s new flow size=%d", __LINE__, __func__,
           FLOW_MANAGE->flows_size);

  FLOW_MANAGE->flows = (mt_module_flow_t **)malloc(FLOW_MANAGE->flows_size *
                                                   sizeof(mt_module_flow_t *));
  for (int i = 0; i < FLOW_MANAGE->flows_size - 1; i++) {
    FLOW_MANAGE->flows[i] = temp_manage->flows[i];
  }
  FLOW_MANAGE->flows_handle =
      (QueueHandle_t *)malloc(FLOW_MANAGE->flows_size * sizeof(QueueHandle_t));

  for (int i = 0; i < FLOW_MANAGE->flows_size - 1; i++) {
    FLOW_MANAGE->flows_handle[i] = temp_manage->flows_handle[i];
  }
  FLOW_MANAGE->flows[FLOW_MANAGE->flows_size - 1] = flow;

  ESP_LOGI(TAG, "free2=%d", esp_get_free_heap_size());
  // return ESP_OK; // debug
  /*QueueHandle_t handle = NULL;
  handle = xQueueCreate(1, sizeof(mt_module_mqtt_msg_t));
  FLOW_MANAGE->flows_handle[FLOW_MANAGE->flows_size - 1] = handle;*/

  FLOW_MANAGE->flows_handle[FLOW_MANAGE->flows_size - 1] =
      xQueueCreate(1, sizeof(mt_module_mqtt_msg_t));

  // return ESP_OK; // debug

  ESP_LOGI(TAG, "%4d %s create flow push task,flow_size=%d,handle=%p", __LINE__,
           __func__, FLOW_MANAGE->flows_size,
           FLOW_MANAGE->flows_handle[FLOW_MANAGE->flows_size - 1]);

  // TODO(ZH) seprate task and new
  // xTaskCreate((TaskFunction_t)mt_module_flow_manage_push_task_loop,
  //           "FLOW_MANAGE_PUSH_TASK", 8 * 1024, flow, 10, NULL);

  if (temp_manage != NULL)
    free(temp_manage);
  return ESP_OK;
}

esp_err_t mt_module_flow_manage_get_index_by_module_id(char *module_id,
                                                       int *module_index) {
  int match = 0;

  if (module_id == NULL) {
    ESP_LOGE(TAG, "%4d %s module_id NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (FLOW_MANAGE == NULL) {
    ESP_LOGE(TAG, "%4d %s FLOW_MANAGE NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  for (int i = 0; i < FLOW_MANAGE->flows_size; i++) {
    if (FLOW_MANAGE->flows[i] == NULL) {
      continue;
    } else {
      if (FLOW_MANAGE->flows[i]->flow == NULL) {
        continue;
      } else {
        if (FLOW_MANAGE->flows[i]->flow->id == NULL) {
          continue;
        } else {
          if (strcmp(FLOW_MANAGE->flows[i]->flow->id, module_id) == 0) {
            *module_index = FLOW_MANAGE->flows[i]->module_index;
            match++;
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

mt_module_flow_t *mt_module_flow_manage_get_flow_by_flowid(int flow_index) {
  if (FLOW_MANAGE == NULL)
    return NULL;

  for (int i = 0; i < FLOW_MANAGE->flows_size; i++) {
    if (FLOW_MANAGE->flows == NULL) {
      continue;
    }

    if (FLOW_MANAGE->flows[i] == NULL) {
      continue;
    }

    if (FLOW_MANAGE->flows[i]->flow_index == flow_index)
      return FLOW_MANAGE->flows[i];
  }

  return NULL;
}
