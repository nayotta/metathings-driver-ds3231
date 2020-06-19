#include "modbus_airswitch001_notify.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "cache_airswitch001.h"
#include "modbus_airswitch001.h"
#include "modbus_airswitch001_module_flow.h"
#include "modbus_airswitch001_module_mqtt.h"
#include "mt_module_flow.h"
#include "mt_module_http.h"
#include "mt_mqtt_lan.h"
#include "mt_mqtt_utils.h"
#include "mt_nvs_storage.h"

// global config ==============================================================

static const char *TAG = "MODBUS_AIRSWITCH001_NOTIFY";

static int CHECK_INTERVAL = 2000; // 2s
static bool STATE[24] = {false};
static bool DELAY_CHECK = false;
static int DELAY_CHECK_TIME = 5000; // 5s

// static func ================================================================

static void modbus_airswitch001_notify_loop() {
  Cache_t *cache = NULL;
  int total_num = 0;
  bool cache_success = false;
  esp_err_t err = ESP_OK;

RESTART:
  // get total switch num
  cache = cache_get();
  if (cache == NULL) {
    ESP_LOGE(TAG, "%4d %s cache_get failed", __LINE__, __func__);
    vTaskDelay(5000 / portTICK_RATE_MS);
    goto RESTART;
  }
  total_num = cache->num_master + cache->num_slaver;

  // cache state
  while (cache_success == false) {
    for (int i = 1; i <= total_num; i++) {
      err = mt_airswitch001_get_state(1, i, &STATE[i]);
      if (err != ESP_OK) {
        ESP_LOGE(TAG, "%4d %s mt_airswitch001_get_state:%d failed", __LINE__,
                 __func__, i);
        vTaskDelay(5000 / portTICK_RATE_MS);
        continue;
      }
    }
    cache_success = true;
  }

  // state check loop
  while (true) {
    bool new_state = false;
    for (int i = 1; i <= total_num; i++) {
      if (DELAY_CHECK == true) {
        vTaskDelay(DELAY_CHECK_TIME / portTICK_RATE_MS);
        DELAY_CHECK = false;
      }
      err = mt_airswitch001_get_state(1, i, &new_state);
      if (err != ESP_OK) {
        ESP_LOGE(TAG, "%4d %s mt_airswitch001_get_state:%d failed", __LINE__,
                 __func__, i);
        vTaskDelay(5000 / portTICK_RATE_MS);
        continue;
      }

      if (new_state != STATE[i]) {
        ESP_LOGI(TAG, "%4d %s local switch:%d from %d to %d", __LINE__,
                 __func__, i, STATE[i], new_state);
        // notify
        mt_module_flow_struct_group_t *group =
            mt_module_flow_new_struct_group_with_notify(2);
        group->value[0]->key = malloc(20);
        sprintf(group->value[0]->key, "state%d", i);
        group->value[0]->type = GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE;
        group->value[0]->bool_value = new_state;

        group->value[1]->key = malloc(20);
        sprintf(group->value[1]->key, "from");
        group->value[1]->type = GOOGLE__PROTOBUF__VALUE__KIND_STRING_VALUE;
        group->value[1]->string_value = malloc(strlen("local") + 1);
        sprintf(group->value[1]->string_value, "local");
        if (module_notify_process(group) != ESP_OK) {
          ESP_LOGE(TAG, "%4d %s module_notify_process failed", __LINE__,
                   __func__);
        }
        mt_module_flow_free_struct_group(group);
      }

      STATE[i] = new_state;
    }
    vTaskDelay(CHECK_INTERVAL / portTICK_RATE_MS);
  }
}

// global func ================================================================

void modbus_airswitch001_notify_task() {
  xTaskCreate((TaskFunction_t)modbus_airswitch001_notify_loop, "notify_loop",
              8 * 1024, NULL, 10, NULL);
}

esp_err_t modbus_airswitch001_notify_update_switch(int addr, bool state) {
  if (addr > 23 || addr < 1) {
    ESP_LOGE(TAG, "%4d %s unexcept addr:%d", __LINE__, __func__, addr);
    return ESP_ERR_INVALID_ARG;
  }

  DELAY_CHECK = true;

  STATE[addr] = state;

  return ESP_OK;
}
