#include "gpio_maglock001.h"
#include "mt_nvs_storage.h"

#include "driver/gpio.h"
#include "stdio.h"

#include "esp_log.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

// global define ==============================================================

static const char *TAG = "GPIO_MAGLOCK001";
static int exist_port_num = 0;
static int exist_port[40];
static int exist_port_on_level[40];
static bool cache_state[40];

// static func ================================================================

static esp_err_t gpio_maglock001_init(int pin, int pin_on_level) {
  esp_err_t err = ESP_OK;

  // check pin value
  if (pin < 0 || pin > 40 - 1) {
    ESP_LOGE(TAG, "%4d %s pin value error:%d", __LINE__, __func__, pin);
    return ESP_ERR_INVALID_ARG;
  }

  // check pin_on_level
  if (!(pin_on_level == 0 || pin_on_level == 1)) {
    ESP_LOGE(TAG, "%4d %s pin_on_level error:%d", __LINE__, __func__,
             pin_on_level);
    return ESP_ERR_INVALID_ARG;
  }

  // gpio init
  gpio_config_t conf;
  conf.intr_type = GPIO_INTR_DISABLE;
  conf.mode = GPIO_MODE_INPUT;
  conf.pin_bit_mask = (1ULL << pin);
  conf.pull_down_en = 0;
  conf.pull_up_en = 1;

  err = gpio_config(&conf);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_config failed:%d", __LINE__, __func__, err);
    return err;
  }

  return ESP_OK;
}

// global func ================================================================

esp_err_t gpio_maglock001_get_port_num(int *num) {
  *num = exist_port_num;

  return ESP_OK;
}

esp_err_t gpio_maglock001_get_state(int port_index, bool *state_out) {
  esp_err_t err = ESP_OK;
  int level = 0;

  level = gpio_get_level(exist_port[port_index]);
  if (level == exist_port_on_level[port_index]) {
    *state_out = true;
  } else {
    *state_out = false;
  }

  return err;
}

esp_err_t gpio_maglock001_task() {
  esp_err_t err = ESP_OK;
  char key[16] = "";
  int maglock_num = 0;

  ESP_LOGI(TAG, "%4d %s init", __LINE__, __func__);

  sprintf(key, "gpio_num");
  if (mt_nvs_read_int32_config(key, &maglock_num) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_read_int32_config:%s failed", __LINE__,
             __func__, key);
    return ESP_ERR_INVALID_ARG;
  }
  exist_port_num = maglock_num;

  for (int i = 0; i < maglock_num; i++) {
    int pin = 0;
    int pin_on_level = 0;

    sprintf(key, "gpio_pin_%d", i + 1);
    if (mt_nvs_read_int32_config(key, &pin) == false) {
      ESP_LOGE(TAG, "%4d %s mt_nvs_read_int32_config:%s failed", __LINE__,
               __func__, key);
      return ESP_ERR_INVALID_ARG;
    }

    sprintf(key, "gpio_on_%d", i + 1);
    if (mt_nvs_read_int32_config(key, &pin_on_level) == false) {
      ESP_LOGE(TAG, "%4d %s mt_nvs_read_int32_config:%s failed", __LINE__,
               __func__, key);
      return ESP_ERR_INVALID_ARG;
    }

    err = gpio_maglock001_init(pin, pin_on_level);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s gpio_maglock001_init failed:%d", __LINE__, __func__,
               err);
      return err;
    }

    exist_port[i + 1] = pin;
    exist_port_on_level[i + 1] = pin_on_level;

    ESP_LOGI(TAG, "%4d %s index:%d pin:%d pin_on_level:%d", __LINE__, __func__,
             i + 1, pin, pin_on_level);
  }

  return ESP_OK;
}

esp_err_t gpio_maglock001_get_has_changed(bool *change) {
  esp_err_t err = ESP_OK;
  *change = false;

  for (int i = 0; i < exist_port_num; i++) {
    bool state_new = false;
    err = gpio_maglock001_get_state(i + 1, &state_new);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s gpio_maglock001_get_state port:%d failed", __LINE__,
               __func__, i + 1);
      return ESP_ERR_INVALID_ARG;
    }

    if (cache_state[i + 1] != state_new) {
      ESP_LOGI(TAG, "%4d %s port:%d changed,new state:%d", __LINE__, __func__,
               i + 1, state_new);
      *change = true;
      cache_state[i + 1] = state_new;
    }
  }

  return ESP_OK;
}

cJSON *gpio_maglock001_get_flow_data() {
  esp_err_t err = ESP_OK;
  cJSON *flow_data = cJSON_CreateObject();
  char key[16] = "";
  bool *state = NULL;

  if (exist_port != 0)
    state = (bool *)malloc(exist_port_num * sizeof(bool));

  for (int i = 0; i < exist_port_num; i++) {
    err = gpio_maglock001_get_state(i + 1, &state[i + 1]);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s gpio_maglock001_get_state index:%d failed",
               __LINE__, __func__, i + 1);
      goto EXIT;
    } else {
      sprintf(key, "state%d", i + 1);
      cJSON_AddBoolToObject(flow_data, key, state[i + 1]);
    }
  }

EXIT:
  if (err != ESP_OK) {
    cJSON_Delete(flow_data);
    flow_data = NULL;
  }
  if (state != NULL) {
    free(state);
  }
  return flow_data;
}
