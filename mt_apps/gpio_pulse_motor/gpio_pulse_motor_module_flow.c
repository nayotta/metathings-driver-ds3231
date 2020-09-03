#include "gpio_pulse_motor_module_flow.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "mt_utils_string.h"

#include "gpio_pulse_motor.h"

// static define ==============================================================

static const char *TAG = "GPIO_PUSLE_MOTOR_MODULE_FLOW";
static mt_module_flow_t *MODULE_FLOW = NULL;

// static func ================================================================

static void gpio_pulse_motor_flow_loop() {
  esp_err_t err = ESP_OK;
  mt_module_flow_struct_group_t *data = NULL;

  while (true) {
    data = gpio_pulse_motor_get_flow_data();
    if (data == NULL) {
      ESP_LOGE(TAG, "%4d %s gpio_pulse_motor_get_flow_data failed", __LINE__,
               __func__);
    }

    err = mt_module_flow_sent_msg(MODULE_FLOW, data);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__,
               __func__);
    }

    mt_module_flow_free_struct_group(data);
    data = NULL;

    vTaskDelay(MODULE_FLOW->push_frame_interval / portTICK_PERIOD_MS);
  }
}

static void
gpio_pulse_motor_module_notify_loop(gpio_pusle_motor_data_t *motor_data) {
  esp_err_t err = ESP_OK;
  mt_module_flow_struct_group_t *data = NULL;

  data = gpio_pulse_motor_get_notify_data(motor_data->type, motor_data->state,
                                          motor_data->time);
  if (data == NULL) {
    ESP_LOGE(TAG, "%4d %s gpio_pulse_motor_get_notify_data failed", __LINE__,
             __func__);
    goto EXIT;
  }

  err = mt_module_flow_sent_msg(MODULE_FLOW, data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__, __func__);
  }

EXIT:
  mt_module_flow_free_struct_group(data);
  gpio_pulse_motor_free_data(motor_data);
  vTaskDelete(NULL);
}

// global func ================================================================

void gpio_pulse_motor_module_flow_task(mt_module_flow_t *module_flow) {
  MODULE_FLOW = module_flow;
  xTaskCreate((TaskFunction_t)gpio_pulse_motor_flow_loop, "MODULE_FLOW",
              8 * 1024, NULL, 10, NULL);
}

void gpio_pulse_motor_module_notify_state_task(char *type, bool state,
                                               float time) {
  gpio_pusle_motor_data_t *data = gpio_pulse_motor_new_data();
  data->type = mt_utils_string_copy(type);
  data->state = state;
  data->time = time;

  xTaskCreate((TaskFunction_t)gpio_pulse_motor_module_notify_loop,
              "MODULE_FLOW", 10 * 1024, data, 10, NULL);
}
