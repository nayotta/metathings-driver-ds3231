#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "mt_nvs_config.h"
#include "mt_utils_string.h"

#include "huawei_mqtt.h"
#include "huawei_mqtt_flow.h"
#include "huawei_mqtt_utils.h"

// global config ==============================================================

static const char *TAG = "HUAWEI_MQTT_FLOW";

// static func ================================================================

static void huawei_mqtt_flow_get_data_process(mt_module_flow_t *module_flow) {
  esp_err_t err = ESP_OK;
  cJSON *data = NULL;
  char *buf = NULL;
  char *clientID = NULL;
  mt_nvs_host_t *host = NULL;

  // check flow handle
  if (module_flow->flow_handle == NULL) {
    ESP_LOGE(TAG, "%4d %s flow_handle NULL", __LINE__, __func__);
    goto EXIT;
  }

  // get data process
  data = module_flow->flow_handle();
  if (data == NULL) {
    ESP_LOGE(TAG, "%4d %s flow_handle exec NULL", __LINE__, __func__);
    goto EXIT;
  }

  // get client ID
  host = mt_nvs_config_get_host_config();
  if (host == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_get_host_config failed", __LINE__,
             __func__);
    goto EXIT;
  }
  clientID = mt_utils_string_copy(host->clientID);
  if (clientID == NULL) {
    ESP_LOGE(TAG, "%4d %s clientID NULL", __LINE__, __func__);
    goto EXIT;
  }

  // gen flow data
  buf = huawei_mqtt_utils_gen_push(data, clientID, "data");

  // push flow data
  err = huawei_mqtt_pub_msg("/datas", buf, strlen(buf));
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s huawei_mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  } else {
    ESP_LOGI(TAG, "%4d %s huawei_mqtt_pub_msg success", __LINE__, __func__);
  }

EXIT:
  if (data != NULL)
    cJSON_Delete(data);
  if (buf != NULL)
    free(buf);
  if (clientID != NULL)
    free(clientID);
  if (host != NULL)
    mt_nvs_config_free_host(host);
}

static void
huawei_mqtt_flow_get_poll_notify_process(mt_module_flow_t *module_flow) {
  esp_err_t err = ESP_OK;
  cJSON *data = NULL;
  char *buf = NULL;
  char *clientID = NULL;
  mt_nvs_host_t *host = NULL;

  // check flow handle
  if (module_flow->poll_process == NULL) {
    ESP_LOGE(TAG, "%4d %s poll_process NULL", __LINE__, __func__);
    goto EXIT;
  }

  // get data process
  data = module_flow->poll_process();
  if (data == NULL) {
    ESP_LOGE(TAG, "%4d %s poll_process exec NULL", __LINE__, __func__);
    goto EXIT;
  }

  // get client ID
  host = mt_nvs_config_get_host_config();
  if (host == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_get_host_config failed", __LINE__,
             __func__);
    goto EXIT;
  }
  clientID = mt_utils_string_copy(host->clientID);
  if (clientID == NULL) {
    ESP_LOGE(TAG, "%4d %s clientID NULL", __LINE__, __func__);
    goto EXIT;
  }

  // gen flow data
  buf = huawei_mqtt_utils_gen_push(data, clientID, "data");

  // push flow data
  err = huawei_mqtt_pub_msg("/datas", buf, strlen(buf));
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s huawei_mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  } else {
    ESP_LOGI(TAG, "%4d %s huawei_mqtt_pub_msg success", __LINE__, __func__);
  }

EXIT:
  if (data != NULL)
    cJSON_Delete(data);
  if (buf != NULL)
    free(buf);
  if (clientID != NULL)
    free(clientID);
  if (host != NULL)
    mt_nvs_config_free_host(host);
}

static void huawei_mqtt_flow_loop(mt_module_flow_t *module_flow) {
  esp_err_t err = ESP_OK;
  int count = 0;

  while (true) {
    if (module_flow->poll_enable == true) {
      bool has_change = false;

      if (module_flow->poll_handle != NULL) {
        err = module_flow->poll_handle(&has_change);
        if (err != ESP_OK) {
          ESP_LOGE(TAG, "%4d %s poll_handle failed", __LINE__, __func__);
          goto ERROR;
        }

        if (has_change == true) {
          huawei_mqtt_flow_get_poll_notify_process(module_flow);
        }
      } else {
        ESP_LOGE(TAG, "%4d %s poll_handle NULL", __LINE__, __func__);
        goto ERROR;
      }

    ERROR:
      if (count * module_flow->poll_interval >
          module_flow->push_frame_interval) {
        huawei_mqtt_flow_get_data_process(module_flow);
        count = 0;
      }
      count++;
      vTaskDelay(module_flow->poll_interval / portTICK_RATE_MS);
    } else {
      if (module_flow->poll_interval > module_flow->push_frame_interval) {
        huawei_mqtt_flow_get_data_process(module_flow);
        vTaskDelay(module_flow->push_frame_interval / portTICK_RATE_MS);
      }
    }
  }
}

// global func  ===============================================================

void huawei_mqtt_flow_task(mt_module_flow_t *module_flow) {
  xTaskCreate((TaskFunction_t)huawei_mqtt_flow_loop, "HUAWEI_MQTT_FLOW",
              8 * 1024, module_flow, 10, NULL);
}
