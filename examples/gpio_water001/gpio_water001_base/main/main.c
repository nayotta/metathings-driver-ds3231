#include "stdint.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "gpio_motor.h"

// global define ==============================================================

static const char *TAG = "GPIO_MOTOR_BASE";

int PINA = 23;
int PINA_ON_LEVEL = 1;
int PINB = 22;
int PINB_ON_LEVEL = 1;

// main func ==================================================================
void app_main() {
  esp_err_t err = ESP_OK;
  bool state = false;

  ESP_LOGI(TAG, "test begin");

  err = gpio_motor_task(PINA, PINA_ON_LEVEL, PINB, PINB_ON_LEVEL);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_motor_task failed", __LINE__, __func__);
    return;
  }

  while (true) {
    ESP_LOGI(TAG, "%4d %s OPEN", __LINE__, __func__);
    gpio_motor_set_state(1);
    gpio_motor_get_state(&state);
    ESP_LOGI(TAG, "%4d %s GET STATE: %d", __LINE__, __func__, state);

    vTaskDelay(5000 / portTICK_RATE_MS);

    ESP_LOGI(TAG, "%4d %s CLOSE", __LINE__, __func__);
    gpio_motor_set_state(0);
    gpio_motor_get_state(&state);
    ESP_LOGI(TAG, "%4d %s GET STATE: %d", __LINE__, __func__, state);

    vTaskDelay(5000 / portTICK_RATE_MS);

    ESP_LOGI(TAG, "%4d %s DELAY CLOSE", __LINE__, __func__);
    gpio_motor_set_delay_close(0.01);
    gpio_motor_get_state(&state);
    ESP_LOGI(TAG, "%4d %s GET STATE: %d", __LINE__, __func__, state);

    vTaskDelay(5000 / portTICK_RATE_MS);

    gpio_motor_get_state(&state);
    ESP_LOGI(TAG, "%4d %s GET STATE: %d", __LINE__, __func__, state);

    vTaskDelay(5000 / portTICK_RATE_MS);
  }
}
