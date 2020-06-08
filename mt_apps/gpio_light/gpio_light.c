#include "stdio.h"

#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "gpio_light.h"

static const char *TAG = "MT_GPIO_LIGHT";

#define GPIO_MAX 40
static bool REG_GPIO_LIGHT[GPIO_MAX] = {false};

// static func ================================================================
static bool mt_gpio_light_init(mt_gpio_light_t *light_handle) {
  int ret = 0;
  int target_level = 0;

  // check pin value
  if (light_handle->pin < 0 || light_handle->pin > GPIO_MAX - 1) {
    ESP_LOGE(TAG, "light use error gpio:%d", light_handle->pin);
    return false;
  }

  // check pin reg
  if (REG_GPIO_LIGHT[light_handle->pin] == true) {
    ESP_LOGE(TAG, "light has been regist:%d", light_handle->pin);
    return false;
  }

  // check pin_on_level
  if (!(light_handle->pin_on_level == 0 || light_handle->pin_on_level == 1)) {
    ESP_LOGE(TAG, "light %d on level set error value:%d", light_handle->pin,
             light_handle->pin_on_level);
    return false;
  }

  // gpio init
  gpio_config_t conf;
  conf.intr_type = GPIO_INTR_DISABLE;
  conf.mode = GPIO_MODE_OUTPUT;
  conf.pin_bit_mask = (1ULL << light_handle->pin);
  conf.pull_down_en = 0;
  conf.pull_up_en = 0;

  ret = gpio_config(&conf);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "light %d gpio_config error", light_handle->pin);
    return false;
  }

  if (light_handle->pin_on_level == 1) {
    target_level = 0;
  } else {
    target_level = 1;
  }
  gpio_set_level(light_handle->pin, target_level);

  REG_GPIO_LIGHT[light_handle->pin] = true;

  return true;
}

static void mt_gpio_light_task_loop(mt_gpio_light_t *light_handle) {
  int count = 0;

  while (1) {
    vTaskDelay(light_handle->blink_interval / portTICK_RATE_MS);
    if (light_handle->blink_en) {
      gpio_set_level(light_handle->pin, count % 2);
      count++;
    }
  }
}

// public func ================================================================
bool mt_gpio_light_set_on(mt_gpio_light_t *light_handle) {
  int ret = 0;
  uint32_t target_level = 0;

  light_handle->blink_en = false;
  if (light_handle->pin_on_level == 1) {
    target_level = 1;
  } else {
    target_level = 0;
  }
  ret = gpio_set_level(light_handle->pin, target_level);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "light %d %s failed", light_handle->pin, __func__);
    return false;
  }

  return true;
}

bool mt_gpio_light_set_off(mt_gpio_light_t *light_handle) {
  int ret = 0;
  uint32_t target_level = 0;

  light_handle->blink_en = false;
  if (light_handle->pin_on_level == 1) {
    target_level = 0;
  } else {
    target_level = 1;
  }
  ret = gpio_set_level(light_handle->pin, target_level);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "light %d %s failed", light_handle->pin, __func__);
    return false;
  }

  return true;
}

bool mt_gpio_light_set_blink(mt_gpio_light_t *light_handle, int intr) {
  // check intr must bigger than 10
  if (intr <= 10) {
    ESP_LOGE(TAG, "light %d check interval error:%d", light_handle->pin, intr);
    return false;
  }

  light_handle->blink_en = true;
  light_handle->blink_interval = intr;

  return true;
}

mt_gpio_light_t *mt_gpio_light_default() {
  mt_gpio_light_t *light_handle = malloc(sizeof(mt_gpio_light_t));

  light_handle->pin = DEFAULT_GPIO_LIGHT_PIN;
  light_handle->pin_on_level = DEFAULT_GPIO_LIGHT_PIN_ON_LEVEL;
  light_handle->blink_en = false;
  light_handle->blink_interval = 2000;

  return light_handle;
}

bool mt_gpio_light_task(mt_gpio_light_t *light_handle) {
  bool ret = false;

  ret = mt_gpio_light_init(light_handle);
  if (ret == false) {
    ESP_LOGI(TAG, "light %d mt_gpio_light_init failed", light_handle->pin);
    return false;
  }

  xTaskCreate((TaskFunction_t)mt_gpio_light_task_loop, "MT_GPIO_LIGHT_TASK",
              1024, light_handle, 10, NULL);
  return true;
}

mt_gpio_light_t *mt_gpio_light_new(int pin, int pin_on_level) {
  mt_gpio_light_t *gpio_light_handle = NULL;
  gpio_light_handle = mt_gpio_light_default();
  gpio_light_handle->pin = pin;
  gpio_light_handle->pin_on_level = pin_on_level;

  return gpio_light_handle;
}
