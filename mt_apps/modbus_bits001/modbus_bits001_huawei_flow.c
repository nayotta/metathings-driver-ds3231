#include "modbus_bits001_huawei_flow.h"

#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "huawei_mqtt.h"
#include "huawei_mqtt_utils.h"
#include "modbus_bits001.h"
#include "mt_nvs_config.h"
#include "mt_utils_string.h"

// global config ==============================================================

static const char *TAG = "HUAWEI_FLOW";

// static func ================================================================

static void
modbus_bits001_huawei_get_ht_data_process(mt_module_flow_t *module_flow) {
  esp_err_t err = ESP_OK;
  cJSON *data = cJSON_CreateObject();
  double temp1 = 25;
  double temp2 = 25;
  double temp3 = 25;
  double hum1 = 70;
  double hum2 = 70;
  double hum3 = 70;
  char *buf = NULL;
  char *clientID = NULL;

  // get data
  // // temp1
  err = modbus_bits001_get_temp(1, &temp1);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_get_temp 1 failed", __LINE__,
             __func__);
  } else {
    cJSON_AddNumberToObject(data, "temp1", temp1);
  }

  // // temp2
  err = modbus_bits001_get_temp(2, &temp2);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_get_temp 2 failed", __LINE__,
             __func__);
  } else {
    cJSON_AddNumberToObject(data, "temp2", temp2);
  }

  // // temp3
  err = modbus_bits001_get_temp(3, &temp3);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_get_temp 3 failed", __LINE__,
             __func__);
  } else {
    cJSON_AddNumberToObject(data, "temp3", temp3);
  }

  // // hum1
  err = modbus_bits001_get_hum(1, &hum1);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_get_hum 1 failed", __LINE__, __func__);
  } else {
    cJSON_AddNumberToObject(data, "hum1", hum1);
  }

  // // hum2
  err = modbus_bits001_get_hum(2, &hum2);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_get_hum 2 failed", __LINE__, __func__);
  } else {
    cJSON_AddNumberToObject(data, "hum2", hum2);
  }

  // // hum3
  err = modbus_bits001_get_hum(3, &hum3);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_get_hum 3 failed", __LINE__, __func__);
  } else {
    cJSON_AddNumberToObject(data, "hum3", hum3);
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

static void
modbus_bits001_huawei_get_fog_data_process(mt_module_flow_t *module_flow) {
  esp_err_t err = ESP_OK;
  cJSON *data = cJSON_CreateObject();
  double fog1 = 0;
  double fog2 = 0;
  double fog3 = 0;
  char *buf = NULL;
  char *clientID = NULL;

  // get data
  // // fog1
  err = modbus_bits001_get_frog(1, &fog1);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_get_frog 1 failed", __LINE__,
             __func__);
  } else {
    cJSON_AddNumberToObject(data, "frog1", fog1);
  }

  // // fog2
  err = modbus_bits001_get_frog(2, &fog2);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_get_frog 2 failed", __LINE__,
             __func__);
  } else {
    cJSON_AddNumberToObject(data, "frog2", fog2);
  }

  // // fog3
  err = modbus_bits001_get_frog(3, &fog3);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_bits001_get_frog 3 failed", __LINE__,
             __func__);
  } else {
    cJSON_AddNumberToObject(data, "frog3", fog3);
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

static void modbus_bits001_huawei_flow_ht_loop(mt_module_flow_t *module_flow) {
  while (true) {
    modbus_bits001_huawei_get_ht_data_process(module_flow);
    vTaskDelay(module_flow->push_frame_interval / portTICK_RATE_MS);
  }
}

static void modbus_bits001_huawei_flow_fog_loop(mt_module_flow_t *module_flow) {
  while (true) {
    modbus_bits001_huawei_get_fog_data_process(module_flow);
    vTaskDelay(module_flow->push_frame_interval / portTICK_RATE_MS);
  }
}

// global func  ===============================================================

void modbus_bits001_huawei_flow_ht_task(mt_module_flow_t *module_flow) {
  xTaskCreate((TaskFunction_t)modbus_bits001_huawei_flow_ht_loop, "HUAWEI_FLOW",
              8 * 1024, module_flow, 10, NULL);
}

void modbus_bits001_huawei_flow_fog_task(mt_module_flow_t *module_flow) {
  xTaskCreate((TaskFunction_t)modbus_bits001_huawei_flow_fog_loop,
              "HUAWEI_FLOW", 8 * 1024, module_flow, 10, NULL);
}
