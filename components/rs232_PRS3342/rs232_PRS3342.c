#include "esp_log.h"
#include "stdio.h"
#include "string.h"

#include "rs232_PRS3342.h"
#include "rs232_dev.h"

// global define ==============================================================

static const char *TAG = "rs232-PRS3342";

// static func ================================================================

static void rs232_PRS3342_parse(uint8_t *buf, int8_t size) {}

static void rs232_PRS3342_loop(rs232_dev_config_t *rs232_config) {
  while (true) {
    int read_size = 0;
    uint8_t *read_buf = NULL;

    read_buf = rs232_dev_read(rs232_config, &read_size);
    if (read_size > 0) {
      ESP_LOGI(TAG, "%4d %s rs232_dev_read, size=%d", __LINE__, __func__,
               read_size);
      for (int i = 0; i < read_size; i++) {
        printf("%2x ", read_buf[i]);
      }
      printf("\n");

      rs232_PRS3342_parse(read_buf, read_size);
    }

    if (read_buf != NULL)
      free(read_buf);
  }
}

// global func ================================================================

esp_err_t rs232_PRS3342_task(rs232_dev_config_t *rs232_config) {
  esp_err_t err = ESP_OK;

  if (rs232_config == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_config NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  err = rs232_dev_init(rs232_config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_dev_init failed", __LINE__, __func__);
    return ESP_ERR_INVALID_STATE;
  }

  xTaskCreate((TaskFunction_t)rs232_PRS3342_loop, "PRS3342_TASK", 8 * 1024,
              rs232_config, 10, NULL);

  return ESP_OK;
}
