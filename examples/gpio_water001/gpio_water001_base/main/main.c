#include "stdint.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "driver/gpio.h"

// global define ==============================================================
static const char *TAG = "GPIO_WATER001_BASE";

int PINA = 12;
int PINB = 14;
int OP_INTERVAL = 1000;

// test func =================================================================
void test_init(int pin) {
  esp_err_t err = ESP_OK;
  gpio_config_t conf;
  conf.intr_type = GPIO_INTR_DISABLE;
  conf.mode = GPIO_MODE_OUTPUT;
  conf.pin_bit_mask = (1ULL << pin);
  conf.pull_down_en = 0;
  conf.pull_up_en = 0;

  err = gpio_config(&conf);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "pin %d gpio_config error", pin);
    return false;
  }

  gpio_set_level(PINA, 0);
}

void test_open() {
  gpio_set_level(PINA, 1);
  gpio_set_level(PINB, 0);
  vTaskDelay(OP_INTERVAL / portTICK_RATE_MS);
  gpio_set_level(PINA, 0);
  gpio_set_level(PINB, 0);
}

void test_close() {
  gpio_set_level(PINA, 0);
  gpio_set_level(PINB, 1);
  vTaskDelay(OP_INTERVAL / portTICK_RATE_MS);
  gpio_set_level(PINA, 0);
  gpio_set_level(PINB, 0);
}

// main func ==================================================================
void app_main() {
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "test begin");

  test_init(PINA);
  test_init(PINB);

  while (true) {
    test_open();
    vTaskDelay(3000 / portTICK_RATE_MS);
    test_close();
    vTaskDelay(3000 / portTICK_RATE_MS);
  }
}
