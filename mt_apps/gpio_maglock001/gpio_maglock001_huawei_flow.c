#include "gpio_maglock001_huawei_flow.h"

#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gpio_maglock001.h"
#include "huawei_mqtt.h"
#include "huawei_mqtt_utils.h"
#include "mt_nvs_config.h"
#include "mt_utils_string.h"

// global config ==============================================================

static const char *TAG = "HUAWEI_FLOW";

// static func ================================================================

static void
gpio_maglock001_huawei_get_data_process(mt_module_flow_t *module_flow) {
  esp_err_t err = ESP_OK;
  cJSON *data = cJSON_CreateObject();
  int port_num = 0;
  char *buf = NULL;
  char *clientID = NULL;

  // get data process
  int module_index = 1;
  err = gpio_maglock001_get_port_num(module_index, &port_num);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_maglock001_get_port_num failed", __LINE__,
             __func__);
    return;
  }

  bool *state = (bool *)malloc(port_num * sizeof(bool));

  for (int i = 0; i < port_num; i++) {
    err = gpio_maglock001_get_state(module_index, i + 1, &state[i]);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s mt_maglock_get_state module:%d index:%d failed",
               __LINE__, __func__, module_index, i + 1);
      return;
    } else {
      cJSON_AddBoolToObject(data, "state1", state[i]);
    }
  }

  // get client ID
  mt_nvs_host_t *host = mt_nvs_config_get_host_config();
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
  cJSON_Delete(data);
  if (buf != NULL)
    free(buf);
  if (clientID != NULL)
    free(clientID);
}

static void gpio_maglock001_huawei_flow_loop(mt_module_flow_t *module_flow) {
  while (true) {
    gpio_maglock001_huawei_get_data_process(module_flow);
    vTaskDelay(module_flow->push_frame_interval / portTICK_RATE_MS);
  }
}

// global func  ===============================================================

void gpio_maglock001_huawei_flow_task(mt_module_flow_t *module_flow) {
  xTaskCreate((TaskFunction_t)gpio_maglock001_huawei_flow_loop, "HUAWEI_FLOW",
              8 * 1024, module_flow, 10, NULL);
}
