#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "gpio_light.h"
#include "gpio_button.h"
#include "mt_nvs_storage.h"
#include "mt_smartconfig.h"
#include "mt_sntp.h"
#include "mt_rtc_time.h"

#define LIGHT_GPIO 26
#define LIGHT_GPIO_ON_LEVEL 0
#define SDA_GPIO 21
#define SCL_GPIO 19

#define BUTTON_GPIO 14
#define BUTTON_GPIO_PRESS_LEVEL 0

static const char *TAG = "MT_SNTP_EXAMPLE";

void example_short_press_callback()
{
  printf("example_short_press\n");
}

void example_long_press_callback()
{
  bool ret = false;

  printf("example_long_press\n");

  ret = mt_nvs_write_string_config("ssid", "");
  if (ret == false)
  {
    ESP_LOGE(TAG, "%d mt_nvs_write_string_config failed", __LINE__);
    return;
  }

  ret = mt_nvs_write_string_config("password", "");
  if (ret == false)
  {
    ESP_LOGE(TAG, "%d mt_nvs_write_string_config failed", __LINE__);
    return;
  }

  fflush(stdout);
  ESP_LOGI(TAG, "reset wifi and reboot now");
  esp_restart();
}

void app_main()
{
  bool ret = false;
  mt_gpio_light_t *gpio_light_handle = NULL;
  mt_gpio_btn_t *gpio_btn_handle = NULL;

  ESP_LOGI(TAG, "\n\n\ntest begin------>");

  ret = mt_nvs_init();
  if (ret == false)
  {
    ESP_LOGE(TAG, "%d mt_nvs_init failed", __LINE__);
    return;
  }

  // config light gpio
  gpio_light_handle = mt_gpio_light_default();
  gpio_light_handle->pin = LIGHT_GPIO;
  gpio_light_handle->pin_on_level = LIGHT_GPIO_ON_LEVEL;

  ret = mt_gpio_light_task(gpio_light_handle);
  if (ret == false)
  {
    ESP_LOGE(TAG, "%d mt_gpio_light_task %d create failed", __LINE__,
             gpio_light_handle->pin);
    return;
  }

  // config button gpio
  gpio_btn_handle = mt_gpio_btn_default();
  gpio_btn_handle->pin = BUTTON_GPIO;
  gpio_btn_handle->pin_on_level = BUTTON_GPIO_PRESS_LEVEL;
  gpio_btn_handle->mt_gpio_btn_short_press_callback = example_short_press_callback;
  gpio_btn_handle->mt_gpio_btn_long_press_callback = example_long_press_callback;

  ret = mt_gpio_btn_task(gpio_btn_handle);
  if (ret == false)
  {
    ESP_LOGE(TAG, "%d mt_gpio_btn_task failed", __LINE__);
    return;
  }

  // config smartconfig
  mt_smartconfig_set_light_handle(gpio_light_handle);
  mt_smartconfig_task();

  // config rtc_time module
  mt_rtc_time_task(MT_RTC_MODULE_DS1307, SDA_GPIO, SCL_GPIO);

  // config sntp task
  mt_sntp_task();

  ESP_LOGI(TAG, "test end------>\n\n");
}
