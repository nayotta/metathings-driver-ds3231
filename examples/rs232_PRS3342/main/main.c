#include "esp_log.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "rs232_PRS3342.h"
#include "rs232_dev.h"

// global define ==============================================================

static const char *TAG = "example-rs232-PRS3342";

// gloabal func ===============================================================

void app_main() {
  esp_err_t err = ESP_OK;
  rs232_dev_config_t *rs232_config = NULL;

  ESP_LOGI(TAG, "test begin");

  rs232_config = rs232_dev_default_new();

  err = rs232_PRS3342_task(rs232_config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_PRS3342_task failed", __LINE__, __func__);
    return;
  }

  ESP_LOGI(TAG, "test end");
}
