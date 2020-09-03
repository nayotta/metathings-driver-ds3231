#include "gpio_pulse_motor_utils.h"

// static define ==============================================================

static const char *TAG = "GPIO_PULSE_MONTOR_UTILS";

// help func ==================================================================

esp_err_t
gpio_pulse_motor_utils_check_set_state_req(MtPulseMotor__SetStateReq *req) {
  if (req == NULL) {
    ESP_LOGE(TAG, "%4d %s req NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}

esp_err_t gpio_pulse_motor_utils_check_set_state_with_delay_req(
    MtPulseMotor__SetStateWithDelayReq *req) {
  if (req == NULL) {
    ESP_LOGE(TAG, "%4d %s req NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (req->time < 0.009) {
    ESP_LOGE(TAG, "%4d %s req->time:%f must bigger than 0.01", __LINE__,
             __func__, req->time);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}
