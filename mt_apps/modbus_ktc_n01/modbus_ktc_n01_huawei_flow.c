#include "modbus_ktc_n01_huawei_flow.h"

#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "huawei_mqtt.h"
#include "huawei_mqtt_utils.h"
#include "modbus_ktc_n01.h"
#include "mt_nvs_config.h"
#include "mt_utils_string.h"

// global config ==============================================================

static const char *TAG = "HUAWEI_FLOW";

// static func ================================================================

static void modbus_ktc_n01_huawei_flow_once(mt_module_flow_t *module_flow) {
  esp_err_t err = ESP_OK;
  cJSON *data = cJSON_CreateObject();
  double temp = 25;
  double hum = 70;
  double current = 0.1;
  char *buf = NULL;
  char *clientID = NULL;

  // get data
  err = modbus_ktc_n01_get_temp_hum(1, &temp, &hum);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_ktc_n01_get_temp_hum failed", __LINE__,
             __func__);
    goto EXIT;
  }

  err = modbus_ktc_n01_get_current(1, &current);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_ktc_n01_get_current failed", __LINE__,
             __func__);
    goto EXIT;
  }

  cJSON_AddNumberToObject(data, "temp", temp);
  cJSON_AddNumberToObject(data, "hum", hum);
  cJSON_AddNumberToObject(data, "current", current);

  // get client ID
  mt_nvs_host_t *host = mt_nvs_config_get_host_config();
  if (host == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_config_get_host_config failed", __LINE__,
             __func__);
    goto EXIT;
  }
  mt_utils_string_copy(host->clientID);
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

static void modbus_ktc_n01_huawei_flow_loop(mt_module_flow_t *module_flow) {
  while (true) {
    modbus_ktc_n01_huawei_flow_once(module_flow);
    vTaskDelay(module_flow->push_frame_interval / portTICK_RATE_MS);
  }
}

// global func  ===============================================================

void modbus_ktc_n01_huawei_flow_task(mt_module_flow_t *module_flow) {
  xTaskCreate((TaskFunction_t)modbus_ktc_n01_huawei_flow_loop, "HUAWEI_FLOW",
              8 * 1024, module_flow, 10, NULL);
}
