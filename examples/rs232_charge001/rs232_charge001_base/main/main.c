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
  esp_err_t err = ESP_OK;
  int32_t res_port = 0;
  int32_t res_result = 0;

  err = rs232_charge001_set_charge(2, 1, 120, &res_port, &res_result);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_set_charge failed", __LINE__,
             __func__);
    return;
  }

  ESP_LOGI(TAG, "%4d %s res_port:%d res_result:%d", __LINE__, __func__,
           res_port, res_result);
}

static void test_get_state() {
  rs232_charge001_state2_t *state = NULL;

  state = rs232_charge001_get_state(2);
  if (state == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_get_state NULL", __LINE__, __func__);
    return;
  }

  ESP_LOGI(TAG, "%4d %s port:%d state:%d lefttime:%d power:%d", __LINE__,
           __func__, state->port, state->state, state->lefttime, state->power);
}

static void test_get_states() {
  rs232_charge001_states_t *states = NULL;

  states = rs232_charge001_get_states();
  if (states == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_charge001_get_states NULL", __LINE__, __func__);
    return;
  }

  ESP_LOGI(TAG, "%4d %s port num:%d", __LINE__, __func__, states->num);
  for (int i = 0; i < states->num; i++) {
    ESP_LOGI(TAG, "%4d %s port:%d state:%d", __LINE__, __func__,
             states->states[i]->port, states->states[i]->state);
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
    test_set_state();
    vTaskDelay(2000 / portTICK_RATE_MS);

    test_get_state();
    vTaskDelay(2000 / portTICK_RATE_MS);

    test_get_states();
    vTaskDelay(2000 / portTICK_RATE_MS);
  }

  ESP_LOGI(TAG, "test end");
}
