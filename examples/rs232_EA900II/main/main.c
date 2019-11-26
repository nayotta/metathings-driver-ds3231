#include "esp_log.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "rs232_EA900II.h"
#include "rs232_dev.h"

// global define ==============================================================

static const char *TAG = "example-rs232-EA900II";

// gloabal func ===============================================================

void app_main() {
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "test begin");

  err = rs232_EA900II_task();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_EA900II_task failed", __LINE__, __func__);
    return;
  }

  ESP_LOGI(TAG, "test end");
}
