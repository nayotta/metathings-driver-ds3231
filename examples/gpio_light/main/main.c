#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "gpio_light.h"

#define TEST_GPIO1 27
#define TEST_GPIO2 26
#define TEST_GPIO3 25
#define TEST_GPIO_ON_LEVEL 0

static const char *TAG = "MT_GPIO_LIGHT_EXAMPLE";

void app_main() {
  bool ret = false;
  mt_gpio_light_t *gpio_light_handle1 = NULL;
  mt_gpio_light_t *gpio_light_handle2 = NULL;
  mt_gpio_light_t *gpio_light_handle3 = NULL;

  ESP_LOGI(TAG, "test begin");

  gpio_light_handle1 = mt_gpio_light_default();
  gpio_light_handle1->pin = TEST_GPIO1;
  gpio_light_handle1->pin_on_level = TEST_GPIO_ON_LEVEL;

  gpio_light_handle2 = mt_gpio_light_default();
  gpio_light_handle2->pin = TEST_GPIO2;
  gpio_light_handle2->pin_on_level = TEST_GPIO_ON_LEVEL;

  gpio_light_handle3 = mt_gpio_light_default();
  gpio_light_handle3->pin = TEST_GPIO3;
  gpio_light_handle3->pin_on_level = TEST_GPIO_ON_LEVEL;

  ret = mt_gpio_light_task(gpio_light_handle1);
  if (ret == false) {
    ESP_LOGE(TAG, "mt_gpio_light_task %d create failed",
             gpio_light_handle1->pin);
    return;
  }

  ret = mt_gpio_light_task(gpio_light_handle2);
  if (ret == false) {
    ESP_LOGE(TAG, "mt_gpio_light_task %d create failed",
             gpio_light_handle2->pin);
    return;
  }

  ret = mt_gpio_light_task(gpio_light_handle3);
  if (ret == false) {
    ESP_LOGE(TAG, "mt_gpio_light_task %d create failed",
             gpio_light_handle3->pin);
    return;
  }

  vTaskDelay(3000 / portTICK_RATE_MS);
  ESP_LOGE(TAG, "mt_gpio_light_set_on");

  ret = mt_gpio_light_set_on(gpio_light_handle1);
  ret = mt_gpio_light_set_on(gpio_light_handle2);
  ret = mt_gpio_light_set_on(gpio_light_handle3);

  vTaskDelay(3000 / portTICK_RATE_MS);
  ESP_LOGE(TAG, "mt_gpio_light_set_blink");

  ret = mt_gpio_light_set_blink(gpio_light_handle1, 1000);
  if (ret == false) {
    ESP_LOGE(TAG, "mt_gpio_light_set_blink %d failed", gpio_light_handle1->pin);
    return;
  }

  ret = mt_gpio_light_set_blink(gpio_light_handle2, 2000);
  if (ret == false) {
    ESP_LOGE(TAG, "mt_gpio_light_set_blink %d failed", gpio_light_handle2->pin);
    return;
  }

  ret = mt_gpio_light_set_blink(gpio_light_handle3, 3000);
  if (ret == false) {
    ESP_LOGE(TAG, "mt_gpio_light_set_blink %d failed", gpio_light_handle3->pin);
    return;
  }

  ESP_LOGI(TAG, "test end");
}
