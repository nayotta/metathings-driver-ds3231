#include "gpio_pulse_motor.h"

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "mt_utils_string.h"

// static define ==============================================================

static const char *TAG = "GPIO_PULSE_MOTOR";
static int PULSE_INTERVAL = 30; // 30 ms
static int PIN_A = -1;
static int PIN_A_ON_LEVEL = 1;
static int PIN_B = -1;
static int PIN_B_ON_LEVEL = 1;
static bool STATE = false;
static TaskHandle_t DELAY_CLOSE_HANDLE = NULL;
static int64_t START_TIME = 0;     // unit: us
static float DELAY_CLOSE_TIME = 0; // uint: s

// static func ================================================================

static esp_err_t gpio_pulse_motor_init(int pin_a, int pin_a_on_level, int pin_b,
                                       int pin_b_on_level) {
  esp_err_t err = ESP_OK;
  gpio_config_t conf;

  // check pin
  if (pin_a < 0 || pin_a > 40 - 1) {
    ESP_LOGE(TAG, "%4d %s pin_a value error:%d", __LINE__, __func__, pin_a);
    return ESP_ERR_INVALID_ARG;
  }
  if (pin_b < 0 || pin_b > 40 - 1) {
    ESP_LOGE(TAG, "%4d %s pin_b value error:%d", __LINE__, __func__, pin_b);
    return ESP_ERR_INVALID_ARG;
  }

  // check pin_on_level
  if (!(pin_a_on_level == 0 || pin_a_on_level == 1)) {
    ESP_LOGE(TAG, "%4d %s pin_a_on_level error:%d", __LINE__, __func__,
             pin_a_on_level);
    return ESP_ERR_INVALID_ARG;
  }
  if (!(pin_b_on_level == 0 || pin_b_on_level == 1)) {
    ESP_LOGE(TAG, "%4d %s pin_b_on_level error:%d", __LINE__, __func__,
             pin_b_on_level);
    return ESP_ERR_INVALID_ARG;
  }

  // gpio A init
  conf.intr_type = GPIO_INTR_DISABLE;
  conf.mode = GPIO_MODE_OUTPUT;
  conf.pin_bit_mask = (1ULL << pin_a);
  conf.pull_down_en = 0;
  conf.pull_up_en = 0;

  err = gpio_config(&conf);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_config A failed:%d", __LINE__, __func__, err);
    return err;
  }

  err = gpio_set_level(pin_a, ~pin_a_on_level);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_set_level A failed:%d", __LINE__, __func__, err);
    return err;
  }

  // gpio B init
  conf.intr_type = GPIO_INTR_DISABLE;
  conf.mode = GPIO_MODE_OUTPUT;
  conf.pin_bit_mask = (1ULL << pin_b);
  conf.pull_down_en = 0;
  conf.pull_up_en = 0;

  err = gpio_config(&conf);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_config B failed:%d", __LINE__, __func__, err);
    return err;
  }

  err = gpio_set_level(pin_b, ~pin_b_on_level);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_set_level B failed:%d", __LINE__, __func__, err);
    return err;
  }

  return ESP_OK;
}

static esp_err_t gpio_pulse_motor_check_init() {
  if (PIN_A < 0) {
    ESP_LOGE(TAG, "%4d %s pin A not init", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (PIN_B < 0) {
    ESP_LOGE(TAG, "%4d %s pin B not init", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}

static void gpio_pulse_motor_delay_reset() {
  START_TIME = 0;
  DELAY_CLOSE_TIME = 0;
}

static void gpio_pulse_motor_delay_close_loop(gpio_pusle_motor_data_t *data) {

  vTaskDelay((data->time * 1000) / portTICK_RATE_MS);
  gpio_pulse_motor_set_state(!data->state);
  gpio_pulse_motor_delay_reset();
  DELAY_CLOSE_HANDLE = NULL;
  ESP_LOGW(TAG, "%4d %s TIME reached", __LINE__, __func__);
  gpio_pulse_motor_free_data(data);
  vTaskDelete(NULL);
}

// help func ==================================================================

gpio_pusle_motor_data_t *gpio_pulse_motor_new_data() {
  gpio_pusle_motor_data_t *data = malloc(sizeof(gpio_pusle_motor_data_t));
  data->type = NULL;
  data->state = false;
  data->time = 0;

  return data;
}

void gpio_pulse_motor_free_data(gpio_pusle_motor_data_t *data) {
  if (data == NULL)
    return;

  if (data->type != NULL)
    free(data->type);

  free(data);
  data = NULL;
}

mt_module_flow_struct_group_t *gpio_pulse_motor_get_flow_data() {
  esp_err_t err = ESP_OK;
  bool state = false;
  float left_time = 0;

  // get state
  err = gpio_pulse_motor_get_state(&state, &left_time);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_pulse_motor_get_state failed", __LINE__,
             __func__);
    return NULL;
  }

  int one_flow_size = 2;
  int count = 0;
  mt_module_flow_struct_group_t *data_out =
      mt_module_flow_new_struct_group(one_flow_size);

  // state
  data_out->value[count]->key = mt_utils_string_copy("state");
  data_out->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE;
  data_out->value[count++]->bool_value = state;

  // leftTime
  data_out->value[count]->key = mt_utils_string_copy("lefTime");
  data_out->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  data_out->value[count++]->number_value = left_time;

  return data_out;
}

mt_module_flow_struct_group_t *
gpio_pulse_motor_get_notify_data(char *type, bool state, float left_time) {
  int one_flow_size = 3;
  int count = 0;
  mt_module_flow_struct_group_t *data_out =
      mt_module_flow_new_struct_group(one_flow_size);

  // type
  data_out->value[count]->key = mt_utils_string_copy("from");
  data_out->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_STRING_VALUE;
  data_out->value[count++]->string_value = mt_utils_string_copy(type);

  // state
  data_out->value[count]->key = mt_utils_string_copy("state");
  data_out->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE;
  data_out->value[count++]->bool_value = state;

  // leftTime
  data_out->value[count]->key = mt_utils_string_copy("lefTime");
  data_out->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  data_out->value[count++]->number_value = left_time;

  return data_out;
}

// global func ================================================================

esp_err_t gpio_pulse_motor_set_state(bool state) {
  esp_err_t err = ESP_OK;

  err = gpio_pulse_motor_check_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_pulse_motor_check_init failed", __LINE__,
             __func__);
    return ESP_ERR_INVALID_ARG;
  }

  // pulse start
  if (state == false) {
    err = gpio_set_level(PIN_A, PIN_A_ON_LEVEL);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s gpio_set_level PIN_A failed", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }

    err = gpio_set_level(PIN_B, !PIN_B_ON_LEVEL);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s gpio_set_level PIN_B failed", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }
  } else {
    err = gpio_set_level(PIN_A, !PIN_A_ON_LEVEL);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s gpio_set_level PIN_A failed", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }

    err = gpio_set_level(PIN_B, PIN_B_ON_LEVEL);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s gpio_set_level PIN_B failed", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }
  }

  // pulse wait
  vTaskDelay(PULSE_INTERVAL / portTICK_RATE_MS);

  // pulse stop
  err = gpio_set_level(PIN_A, ~PIN_A_ON_LEVEL);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_set_level PIN_A failed", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  err = gpio_set_level(PIN_B, ~PIN_A_ON_LEVEL);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_set_level PIN_B failed", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  STATE = state;

  return ESP_OK;
}

esp_err_t gpio_pulse_motor_get_state(bool *state_out, float *left_time) {
  *state_out = STATE;

  if (DELAY_CLOSE_TIME != 0) {
    *left_time =
        DELAY_CLOSE_TIME - (esp_timer_get_time() - START_TIME) / 1000000.0;
  } else {
    *left_time = 0;
  }
  return ESP_OK;
}

esp_err_t gpio_pulse_motor_set_state_with_delay(bool state, float time) {
  esp_err_t err = ESP_OK;
  gpio_pusle_motor_data_t *data = gpio_pulse_motor_new_data();

  // check arg
  if (time < 0.009) {
    ESP_LOGE(TAG, "%4d %s time:%f must bigger than 0.01", __LINE__, __func__,
             time);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // check if has task running
  if (DELAY_CLOSE_HANDLE != NULL) {
    vTaskDelete(DELAY_CLOSE_HANDLE);
    DELAY_CLOSE_HANDLE = NULL;
  }

  err = gpio_pulse_motor_set_state(state);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_pulse_motor_set_state failed", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  data->state = state;
  data->time = time;
  xTaskCreate((TaskFunction_t)gpio_pulse_motor_delay_close_loop,
              "DELAY_CLOSE_TASK", 2 * 1024, data, 10, DELAY_CLOSE_HANDLE);

  START_TIME = esp_timer_get_time();
  DELAY_CLOSE_TIME = time;

EXIT:
  return err;
}

esp_err_t gpio_pulse_motor_task(int pin_a, int pin_a_on_level, int pin_b,
                                int pin_b_on_level) {
  esp_err_t err = ESP_OK;

  err = gpio_pulse_motor_init(pin_a, pin_a_on_level, pin_b, pin_b_on_level);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_pulse_motor_init failed:%d", __LINE__, __func__,
             err);
    return err;
  }

  PIN_A = pin_a;
  PIN_B = pin_b;
  PIN_A_ON_LEVEL = pin_a_on_level;
  PIN_B_ON_LEVEL = pin_b_on_level;

  return ESP_OK;
}
