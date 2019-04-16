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
  mt_gpio_btn_t *gpio_btn_handle = NULL;

  ESP_LOGI(TAG, "test begin");

  gpio_btn_handle = mt_gpio_btn_default();
  gpio_btn_handle->pin = TEST_GPIO;
  gpio_btn_handle->pin_on_level = TEST_GPIO_PRESS_LEVEL;
  gpio_btn_handle->mt_gpio_btn_short_press_callback = example_short_press_callback;
  gpio_btn_handle->mt_gpio_btn_long_press_callback = example_long_press_callback;

  ret = mt_gpio_btn_task(gpio_btn_handle);
  if (ret == false)
  {
    ESP_LOGE(TAG, "mt_gpio_btn_new error new");
    return;
  }

  ESP_LOGI(TAG, "test end");
}
