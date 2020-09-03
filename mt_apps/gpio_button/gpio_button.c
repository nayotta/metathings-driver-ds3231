#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "stdio.h"

#include "gpio_button.h"

// static define ==============================================================

static const char *TAG = "GPIO_BTN";

#define GPIO_MAX 40
static bool REG_GPIO_BTN[GPIO_MAX] = {false};
static mt_gpio_btn_t *GPIO_BTN_HDL[GPIO_MAX];

// static func ================================================================

static void gpio_btn_default_short_press_callback() {
  ESP_LOGI(TAG, "%4d %s default callback, nothing to do", __LINE__, __func__);
}

static void gpio_btn_default_long_press_callback() {
  ESP_LOGI(TAG, "%4d %s default callback, nothing to do", __LINE__, __func__);
}
static void gpio_btn_default_double_press_callback() {
  ESP_LOGI(TAG, "%4d %s default callback, nothing to do", __LINE__, __func__);
}

static bool mt_gpio_btn_init(mt_gpio_btn_t *btn_handle) {
  int ret = 0;

  // check pin value
  if (btn_handle->pin < 0 || btn_handle->pin > GPIO_MAX - 1) {
    ESP_LOGE(TAG, "%4d %s button use error gpio:%d", __LINE__, __func__,
             btn_handle->pin);
    return false;
  }

  // check pin reg
  if (REG_GPIO_BTN[btn_handle->pin] == true) {
    ESP_LOGE(TAG, "%4d %s button has been regist:%d", __LINE__, __func__,
             btn_handle->pin);
    return false;
  }

  // check gpio_on_level
  if (!(btn_handle->pin_on_level == 0 || btn_handle->pin_on_level == 1)) {
    ESP_LOGE(TAG, "%4d %s button %d on level set error value:%d", __LINE__,
             __func__, btn_handle->pin, btn_handle->pin_on_level);
    return false;
  }

  // set press interval default
  if (btn_handle->short_press_interval == 0) {
    btn_handle->short_press_interval = DEFAULT_SHORT_PRESS_INTERVAL;
  }
  if (btn_handle->long_press_interval == 0) {
    btn_handle->long_press_interval = DEFAULT_LONG_PRESS_INTERVAL;
  }
  if (btn_handle->double_press_interval == 0) {
    btn_handle->double_press_interval = DEFAULT_DOUBLE_PRESS_INTERVAL;
  }

  // check press interval value
  if (btn_handle->short_press_interval <= 0 ||
      btn_handle->long_press_interval <= 0 ||
      btn_handle->double_press_interval <= 0 ||
      btn_handle->short_press_interval > btn_handle->long_press_interval) {
    ESP_LOGE(
        TAG,
        "%4d %sbutton %d check press interval value error:shot=%d, long=%d, "
        "double=%d",
        __LINE__, __func__, btn_handle->pin, btn_handle->short_press_interval,
        btn_handle->long_press_interval, btn_handle->double_press_interval);
    return false;
  }

  // check callback fun set
  if (btn_handle->mt_gpio_btn_short_press_callback == NULL) {
    ESP_LOGE(
        TAG,
        "%4d %s button %d callback mt_gpio_btn_50ms_press_callback not set",
        __LINE__, __func__, btn_handle->pin);
    return false;
  }

  if (btn_handle->mt_gpio_btn_long_press_callback == NULL) {
    ESP_LOGE(TAG,
             "%4d %s button %d callback mt_gpio_btn_5s_press_callback not set",
             __LINE__, __func__, btn_handle->pin);
    return false;
  }

  if (btn_handle->mt_gpio_btn_double_press_callback == NULL) {
    ESP_LOGE(
        TAG,
        "%4d %s button %d callback mt_gpio_btn_double_press_callback not set",
        __LINE__, __func__, btn_handle->pin);
    return false;
  }

  // gpio init
  gpio_config_t conf;
  conf.intr_type = GPIO_INTR_DISABLE;
  conf.mode = GPIO_MODE_INPUT;
  conf.pin_bit_mask = (1ULL << btn_handle->pin);
  if (btn_handle->pin_on_level == 1) {
    conf.pull_down_en = 1;
    conf.pull_up_en = 0;
  } else {
    conf.pull_down_en = 0;
    conf.pull_up_en = 1;
  }
  ret = gpio_config(&conf);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s button %d gpio_config error", __LINE__, __func__,
             btn_handle->pin);
    return false;
  }

  // regist button
  REG_GPIO_BTN[btn_handle->pin] = true;
  GPIO_BTN_HDL[btn_handle->pin] = btn_handle;

  return true;
}

static void mt_gpio_btn_task_loop(mt_gpio_btn_t *btn_handle) {
  int level = 0;
  uint32_t sum = 0;            // press count
  uint32_t none_press_sum = 0; // none press count for double press
  int interval = 10;           // task interval do not less than 10
  int short_press_count =
      (int)(btn_handle->short_press_interval / interval); // short press count
  int long_press_count =
      (int)(btn_handle->long_press_interval / interval); // long press count
  int double_press_count =
      (int)(btn_handle->double_press_interval / interval); // double press count

  while (1) {
    level = gpio_get_level(btn_handle->pin);
    if (level == btn_handle->pin_on_level) {
      if (sum == short_press_count) {
      }
      if (sum == long_press_count) {
        ESP_LOGI(TAG, "%4d %s button %d long press", __LINE__, __func__,
                 btn_handle->pin);
        btn_handle->mt_gpio_btn_long_press_callback();
      }
      sum++;
    } else {
      if (sum >= long_press_count) {
        none_press_sum = 0;
      } else if (sum >= short_press_count) {
        if (none_press_sum < double_press_count) {
          if (none_press_sum > short_press_count / 2) {
            ESP_LOGI(TAG, "%4d %s button %d double press", __LINE__, __func__,
                     btn_handle->pin);
            btn_handle->mt_gpio_btn_double_press_callback();
          } else {
            ESP_LOGE(TAG, "%4d %s button %d ignore double press", __LINE__,
                     __func__, btn_handle->pin);
          }
          none_press_sum = double_press_count;
        } else {
          ESP_LOGI(TAG, "%4d %s button %d short press", __LINE__, __func__,
                   btn_handle->pin);
          btn_handle->mt_gpio_btn_short_press_callback();
          none_press_sum = 0;
        }
      } else if (sum > 0) {
        ESP_LOGI(TAG, "%4d %s button %d error press", __LINE__, __func__,
                 btn_handle->pin);
      }
      sum = 0;
    }
    none_press_sum++;
    vTaskDelay(interval / portTICK_RATE_MS);
  }
}

// public func ================================================================

mt_gpio_btn_t *mt_gpio_btn_default() {
  mt_gpio_btn_t *gpio_btn_handle = malloc(sizeof(mt_gpio_btn_t));

  gpio_btn_handle->pin = DEFAULT_GPIO_BTN_PIN;
  gpio_btn_handle->pin_on_level = DEFAULT_GPIO_BTN_PIN_ON_LEVEL;
  gpio_btn_handle->short_press_interval = DEFAULT_SHORT_PRESS_INTERVAL;
  gpio_btn_handle->long_press_interval = DEFAULT_LONG_PRESS_INTERVAL;
  gpio_btn_handle->double_press_interval = DEFAULT_DOUBLE_PRESS_INTERVAL;
  gpio_btn_handle->mt_gpio_btn_short_press_callback =
      gpio_btn_default_short_press_callback;
  gpio_btn_handle->mt_gpio_btn_long_press_callback =
      gpio_btn_default_long_press_callback;
  gpio_btn_handle->mt_gpio_btn_double_press_callback =
      gpio_btn_default_double_press_callback;

  return gpio_btn_handle;
}

bool mt_gpio_btn_task(mt_gpio_btn_t *btn_handle) {
  bool ret = false;

  ret = mt_gpio_btn_init(btn_handle);
  if (ret == false) {
    ESP_LOGE(TAG, "%4d %s error new", __LINE__, __func__);
    return false;
  }
  xTaskCreate((TaskFunction_t)mt_gpio_btn_task_loop, "GPIO_BTN_TASK", 1024 * 2,
              btn_handle, 10, NULL);
  return true;
}

mt_gpio_btn_t *gpio_btn_get_handle(int pin) {
  if (pin < 0 || pin > GPIO_MAX) {
    ESP_LOGE(TAG, "%4d %s error pin:%d", __LINE__, __func__, pin);
    return NULL;
  }

  if (REG_GPIO_BTN[pin] == true) {
    return GPIO_BTN_HDL[pin];
  } else {
    return NULL;
  }
}
