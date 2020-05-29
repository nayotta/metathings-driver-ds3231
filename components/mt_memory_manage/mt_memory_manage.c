#include "mt_memory_manage.h"

#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/task.h"

#include "stdio.h"

#include "esp_log.h"

// static define ==============================================================

static const char *TAG = "MT_MEMORY_MANAGE";

// static func ================================================================

void mt_menory_manage_loop(bool debug) {
  while (1) {
    if (debug == true) {
      ESP_LOGI(TAG, "free=%d", esp_get_free_heap_size());
    }

    if (esp_get_free_heap_size() < 10000) {
      ESP_LOGE(TAG, "memory too low:%d restart now!!!!!",
               esp_get_free_heap_size());
      esp_restart();
    }

    vTaskDelay(2000 / portTICK_RATE_MS);
  }
}

// global func ================================================================

void mt_memory_manage_task(bool debug) {
  xTaskCreate((TaskFunction_t)mt_menory_manage_loop, "MT_MOMORY_MANAGE_TASK",
              100, debug, 2, NULL);
}
