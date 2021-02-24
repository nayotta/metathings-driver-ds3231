#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "rs232_charge001.h"
#include "rs232_dev.h"

// global define ==============================================================

static const char *TAG = "rs232_charge001_base";

static int UART_NUM = 2;
static int RX_PIN = 15;
static int TX_PIN = 13;

// static func ================================================================

static void test_set_state() {
  int64_t begin = esp_timer_get_time() / 1000000;
  esp_err_t err = ESP_OK;
  int32_t res_port = 0;
  int32_t res_result = 0;

  err = rs232_charge001_set_charge(UART_NUM, 2, 240, &res_port, &res_result);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_set_charge failed", __LINE__,
             __func__);
    return;
  }

  int64_t end = esp_timer_get_time() / 1000000;

  ESP_LOGI(TAG, "%4d %s time:%4lld res_port:%d res_result:%d", __LINE__,
           __func__, end - begin, res_port, res_result);
}

static void test_get_state(int32_t port) {
  int64_t begin = esp_timer_get_time() / 1000000;
  rs232_charge001_state2_t *state = NULL;

  state = rs232_charge001_get_state(port);
  if (state == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_get_state NULL", __LINE__, __func__);
    return;
  }

  int64_t end = esp_timer_get_time() / 1000000;

  ESP_LOGI(TAG, "%4d %s time:%4lld port:%d state:%d lefttime:%d power:%d",
           __LINE__, __func__, end - begin, state->port, state->state,
           state->lefttime, state->power);
}

static void test_get_states() {
  int64_t begin = esp_timer_get_time() / 1000000;
  rs232_charge001_states_t *states = NULL;

  states = rs232_charge001_get_states();
  if (states == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_get_states NULL", __LINE__, __func__);
    return;
  }

  int64_t end = esp_timer_get_time() / 1000000;

  ESP_LOGI(TAG, "%4d %s time:%4lld port num:%d", __LINE__, __func__,
           end - begin, states->num);
  for (int i = 0; i < states->num; i++) {
    ESP_LOGI(TAG, "%4d %s port:%d state:%d", __LINE__, __func__,
             states->states[i]->port, states->states[i]->state);
  }
}

static void test_get_states2() {
  int64_t begin = esp_timer_get_time() / 1000000;
  rs232_charge001_states2_t *states = NULL;
  states = rs232_charge001_get_states2();
  if (states == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_get_states NULL", __LINE__, __func__);
    return;
  }

  int64_t end = esp_timer_get_time() / 1000000;

  ESP_LOGI(TAG, "%4d %s time:%4lld port num:%d", __LINE__, __func__,
           end - begin, states->num);
  for (int i = 0; i < states->num; i++) {
    if (states->states[i] != NULL) {
      ESP_LOGI(TAG, "%4d %s port:%d state:%d lefttime:%4d power:%4d", __LINE__,
               __func__, states->states[i]->port, states->states[i]->state,
               states->states[i]->lefttime, states->states[i]->power);
    } else {
      ESP_LOGE(TAG, "%4d %s index:%d NULL", __LINE__, __func__, i + 1);
    }
  }
}

// gloabal func ===============================================================

void app_main() {
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "test begin");

  // serial init
  err = rs232_charge001_init(UART_NUM, RX_PIN, TX_PIN);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_init failed", __LINE__, __func__);
    return;
  }

  // loop
  while (true) {
    // for (int i = 0; i < 10; i++) {
    //   printf("test set\n");
    // test_set_state();
    // vTaskDelay(5000 / portTICK_RATE_MS);
    // }

    for (int i = 0; i < 10; i++) {
      test_get_state(i + 1);
      vTaskDelay(10000 / portTICK_RATE_MS);
    }

    // test_get_states();
    // vTaskDelay(10000 / portTICK_RATE_MS);

    // test_get_states2();
    // vTaskDelay(10000 / portTICK_RATE_MS);
  }

  ESP_LOGI(TAG, "test end");
}
