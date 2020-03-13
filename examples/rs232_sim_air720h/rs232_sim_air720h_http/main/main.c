#include "esp_log.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "rs232_dev.h"
#include "rs232_sim_air720h.h"

#include "mt_module_http.h"

#include "esp_heap_trace.h"
#define NUM_RECORDS 100
static heap_trace_record_t trace_record[NUM_RECORDS];

// global define ==============================================================

static const char *TAG = "rs232_sim_air720h_http";

static int UART_NUM = 2;
static int BAUD_RATE = 115200;
static int RX_PIN = 16;
static int TX_PIN = 17;

// gloabal func ===============================================================

void app_main() {
  esp_err_t err = ESP_OK;
  mt_module_http_t *module_http = NULL;

  ESP_LOGI(TAG, "test begin");

  ESP_ERROR_CHECK(heap_trace_init_standalone(trace_record, NUM_RECORDS));

  // serial init
  err = rs232_sim_air720h_serial_init(UART_NUM, RX_PIN, TX_PIN, BAUD_RATE);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_serial_init failed", __LINE__,
             __func__);
    return;
  }

  // metathings module http task
  module_http = mt_module_http_new(1);
  if (module_http == NULL) {
    ESP_LOGE(TAG, "%4d %s mt_module_http_new failed", __LINE__, __func__);
    return;
  }

  // test http init
  ESP_ERROR_CHECK(heap_trace_start(HEAP_TRACE_LEAKS));
  while (rs232_sim_air720h_http_init(module_http) != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_init failed", __LINE__,
             __func__);
    vTaskDelay(10000 / portTICK_RATE_MS);
  }
  ESP_ERROR_CHECK(heap_trace_stop());
  heap_trace_dump();

  vTaskDelay(2000 / portTICK_RATE_MS);

  // test issue token
  ESP_ERROR_CHECK(heap_trace_start(HEAP_TRACE_LEAKS));
  while (rs232_sim_air720h_http_issue_module_token(module_http) != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_issue_module_token failed",
             __LINE__, __func__);
    vTaskDelay(20000 / portTICK_RATE_MS);
  }
  ESP_ERROR_CHECK(heap_trace_stop());
  heap_trace_dump();

  // test show module
  ESP_ERROR_CHECK(heap_trace_start(HEAP_TRACE_LEAKS));
  while (rs232_sim_air720h_http_show_module(module_http) != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_show_module failed", __LINE__,
             __func__);
    vTaskDelay(20000 / portTICK_RATE_MS);
  }
  ESP_ERROR_CHECK(heap_trace_stop());
  heap_trace_dump();

  // test heartbeat
  while (true) {
    ESP_ERROR_CHECK(heap_trace_start(HEAP_TRACE_LEAKS));
    err = rs232_sim_air720h_http_heartbeat(module_http);
    if (err != ESP_OK)
      ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_http_show_module failed",
               __LINE__, __func__);
    ESP_ERROR_CHECK(heap_trace_stop());
    heap_trace_dump();

    vTaskDelay(20000 / portTICK_RATE_MS);
  }

  ESP_LOGI(TAG, "test end");
}
