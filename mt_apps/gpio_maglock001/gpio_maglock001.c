#include "gpio_maglock001.h"
#include "mt_nvs_storage.h"

// global define ==============================================================
static const char *TAG = "GPIO_MAGLOCK001";
static int cache_state[40];

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
  conf.pull_down_en = 1;
  conf.pull_up_en = 0;

  err = gpio_config(&conf);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_config failed:%d", __LINE__, __func__, err);
    return err;
  }

  return ESP_OK;
}

// global func ================================================================
esp_err_t gpio_maglock001_get_port_num(int module_index, int *num_out) {
  char key[16] = "";

  sprintf(key, "mod_%d_num", module_index);
  if (mt_nvs_read_int32_config(key, num_out) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_read_int32_config:%s failed", __LINE__,
             __func__, key);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}

esp_err_t gpio_maglock001_get_state(int module_index, int port_index,
                                    bool *state_out) {
  esp_err_t err = ESP_OK;
  int level = 0;
  char key[16] = "";
  int pin = 0;
  int pin_on_level;

  sprintf(key, "mod_%d_port_%d", module_index, port_index);
  if (mt_nvs_read_int32_config(key, &pin) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_read_int32_config:%s failed", __LINE__,
             __func__, key);
    return ESP_ERR_INVALID_ARG;
  }

  sprintf(key, "mod_%d_on_%d", module_index, port_index);
  if (mt_nvs_read_int32_config(key, &pin_on_level) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_read_int32_config:%s failed", __LINE__,
             __func__, key);
    return ESP_ERR_INVALID_ARG;
  }

  level = gpio_get_level(pin);
  if (level == pin_on_level) {
    *state_out = true;
  } else {
    *state_out = false;
  }

  return err;
}

esp_err_t gpio_maglock001_task(int module_index) {
  esp_err_t err = ESP_OK;
  char key[16] = "";
  int maglock_num = 0;

  ESP_LOGI(TAG, "%4d %s init", __LINE__, __func__);

  sprintf(key, "mod_%d_num", module_index);
  if (mt_nvs_read_int32_config(key, &maglock_num) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_read_int32_config:%s failed", __LINE__,
             __func__, key);
    return ESP_ERR_INVALID_ARG;
  }

  for (int i = 0; i < maglock_num; i++) {
    int pin = 0;
    int pin_on_level = 0;

    sprintf(key, "mod_%d_port_%d", module_index, i + 1);
    if (mt_nvs_read_int32_config(key, &pin) == false) {
      ESP_LOGE(TAG, "%4d %s mt_nvs_read_int32_config:%s failed", __LINE__,
               __func__, key);
      return ESP_ERR_INVALID_ARG;
    }

    sprintf(key, "mod_%d_on_%d", module_index, i + 1);
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

    ESP_LOGI(TAG, "%4d %s module:%d index:%d pin:%d pin_on_level:%d", __LINE__,
             __func__, module_index, i + 1, pin, pin_on_level);
  }

  return ESP_OK;
}

esp_err_t gpio_maglock001_get_has_changed(int module_index, bool *change) {
  int port_num = 0;
  esp_err_t err = ESP_OK;
  *change = false;

  err = gpio_maglock001_get_port_num(module_index, &port_num);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_maglock001_get_port_num module:%d failed",
             __LINE__, __func__, module_index);
    return ESP_ERR_INVALID_ARG;
  }

  for (int i = 0; i < port_num; i++) {
    bool state_new = false;
    err = gpio_maglock001_get_state(module_index, i + 1, &state_new);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s gpio_maglock001_get_state module:%d port:%d failed",
               __LINE__, __func__, module_index, i + 1);
      return ESP_ERR_INVALID_ARG;
    }

    if (cache_state[i + 1] != state_new) {
      ESP_LOGI(TAG, "%4d %s module:%d port:%d changed,new state:%d", __LINE__,
               __func__, module_index, i + 1, state_new);
      *change = true;
      cache_state[i + 1] = state_new;
    }
  }

  return ESP_OK;
}
