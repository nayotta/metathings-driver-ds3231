#include "modbus_reclose001_module_flow.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "modbus_reclose001.h"
#include "mt_module_flow.h"

// static define ==============================================================

static const char *TAG = "MODBUS_RECLOSE001_MODULE_FLOW";

// static func ================================================================

static void modbus_reclose001_push_data_process(mt_module_flow_t *module_flow,
                                                modbus_reclose001_data_t *data,
                                                bool check) {
  esp_err_t err = ESP_OK;
  mt_module_flow_struct_group_t *group = NULL;
  int count = 0;

  // data struct
  int struct_size = 0;
  if (check == true) {
    if (data->if_change_state == true) {
      struct_size++;
    }
    if (data->if_change_warn_high_voltage == true) {
      struct_size++;
    }
    if (data->if_change_warn_low_voltage == true) {
      struct_size++;
    }
    if (data->if_change_warn_overload == true) {
      struct_size++;
    }
    if (data->if_change_warn_short == true) {
      struct_size++;
    }
    if (data->if_change_warn_leak == true) {
      struct_size++;
    }
  } else {
    struct_size = 6;
  }
  ESP_LOGW(TAG, "%4d %s check:%d, size:%d", __LINE__, __func__, check,
           struct_size);
  group = mt_module_flow_new_struct_group(struct_size);

  // state
  if ((check == true && data->if_change_state == true) || check == false)
    mt_module_flow_set_bool_value(group->value[count++], "state0", data->state);

  // warnHighVoltage0
  if ((check == true && data->if_change_warn_high_voltage == true) ||
      check == false)
    mt_module_flow_set_bool_value(group->value[count++], "warnHighVoltage0",
                                  data->warn_high_voltage);

  // warnLowVoltage0
  if ((check == true && data->if_change_warn_low_voltage == true) ||
      check == false)
    mt_module_flow_set_bool_value(group->value[count++], "warnLowVoltage0",
                                  data->warn_low_voltage);

  // warnOverload0
  if ((check == true && data->if_change_warn_overload == true) ||
      check == false)
    mt_module_flow_set_bool_value(group->value[count++], "warnOverload0",
                                  data->warn_overload);

  // warnShort0
  if ((check == true && data->if_change_warn_short == true) || check == false)
    mt_module_flow_set_bool_value(group->value[count++], "warnShort0",
                                  data->warn_short);

  // warnLeak0
  if ((check == true && data->if_change_warn_leak == true) || check == false)
    mt_module_flow_set_bool_value(group->value[count++], "warnLeak0",
                                  data->warn_leak);

  // send msg
  err = mt_module_flow_sent_msg(module_flow, group);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  // log
  ESP_LOGI(TAG, "%4d %s session:%s sent success", __LINE__, __func__,
           module_flow->session);
  for (int j = 0; j < group->size; j++) {
    ESP_LOGI(TAG, "%4d %s key:%s value:%d", __LINE__, __func__,
             group->value[j]->key, group->value[j]->bool_value);
  }

EXIT:
  if (group != NULL)
    mt_module_flow_free_struct_group(group);

  return;
}

// global func ================================================================

static void modbus_reclose001_flow_loop(mt_module_flow_t *module_flow) {
  int64_t begin = esp_timer_get_time() / 1000000;
  int64_t end = 0;
  esp_err_t err = ESP_OK;
  modbus_reclose001_data_t *data_save = modbus_reclose001_new_data();
  modbus_reclose001_data_t *data = modbus_reclose001_new_data();
  bool change = false;

  while (true) {
    err = modbus_reclose001_get_data(1, data);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s modbus_reclose001_get_data failed", __LINE__,
               __func__);
      goto EXIT;
    }

    err = modbus_reclose001_get_data_and_check_if_change(1, data, data_save,
                                                         &change);
    if (err != ESP_OK) {
      ESP_LOGE(TAG,
               "%4d %s modbus_reclose001_get_data_and_check_if_change failed",
               __LINE__, __func__);
      goto EXIT;
    }

    if (change == true) {
      modbus_reclose001_push_data_process(module_flow, data_save, true);
      begin = esp_timer_get_time() / 1000000;
    }

    end = esp_timer_get_time() / 1000000;
    if (end - begin > (module_flow->push_frame_interval) / 1000) {
      modbus_reclose001_push_data_process(module_flow, data_save, false);
      begin = esp_timer_get_time() / 1000000;
    }

  EXIT:
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void modbus_reclose001_module_flow_task(mt_module_flow_t *module_flow) {
  xTaskCreate((TaskFunction_t)modbus_reclose001_flow_loop, "MODULE_FLOW",
              8 * 1024, module_flow, 10, NULL);
}
