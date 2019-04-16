#include "esp_log.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "gpio_button.h"

#define TEST_GPIO 14
#define TEST_GPIO_PRESS_LEVEL 0

static const char *TAG = "MT_GPIO_BUTTON_EXAMPLE";

void example_short_press_callback()
{
  printf("example_short_press\n");
}

void example_long_press_callback()
{
  printf("example_long_press\n");
}

void app_main()
{
  bool ret = false;

  ESP_LOGI(TAG, "test begin");

  mt_gpio_btn_t *gpio_btn_handle = malloc(sizeof(mt_gpio_btn_t));
  gpio_btn_handle->pin = TEST_GPIO;
  gpio_btn_handle->pin_on_level = TEST_GPIO_PRESS_LEVEL;
  gpio_btn_handle->short_press_interval = SHORT_PRESS_INTERVAL_DEFAULT;
  gpio_btn_handle->long_press_interval = LONG_PRESS_INTERVAL_DEFAULT;
  gpio_btn_handle->double_press_interval = DOUBLE_PRESS_INTERVAL_DEFAULT;
  gpio_btn_handle->mt_gpio_btn_short_press_callback = example_short_press_callback;
  gpio_btn_handle->mt_gpio_btn_long_press_callback = example_long_press_callback;

  ret = mt_btn_task(gpio_btn_handle);
  if (ret == false)
  {
    ESP_LOGE(TAG, "mt_btn_task error create");
    return;
  }

  ESP_LOGI(TAG, "test end");
}
