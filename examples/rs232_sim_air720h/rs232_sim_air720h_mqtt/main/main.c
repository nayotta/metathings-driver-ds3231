#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "rs232_dev.h"
#include "rs232_sim_air720h.h"

#include "mt_module_flow_manage.h"
#include "mt_module_http.h"
#include "mt_module_http_utils.h"
#include "mt_utils_session.h"

#include "esp_heap_trace.h"
#define NUM_RECORDS 100
static heap_trace_record_t trace_record[NUM_RECORDS];

// global define ==============================================================

static const char *TAG = "rs232_sim_air720h_http";

static int UART_NUM = 2;
static int BAUD_RATE = 115200;
static int RX_PIN = 15;
static int TX_PIN = 13;

static char *device_id = "9c8dc3454c5643988cb9da831471f485";
static char *module_id = "693180ceb6cd4cb3b81e0d408a20e42e";

// static func ================================================================
static void test_handle(char *topic, char *buf, int size) {
  printf("no handle\n");
}

// gloabal func ===============================================================

void app_main() {
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "test begin");

  // ESP_ERROR_CHECK(heap_trace_init_standalone(trace_record, NUM_RECORDS));

  // serial init
  err = rs232_sim_air720h_serial_init(UART_NUM, RX_PIN, TX_PIN, BAUD_RATE);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_sim_air720h_serial_init failed", __LINE__,
             __func__);
    return;
  }

  // metathings module http task
  rs232_sim_air720h_mqtt_task(module_id, device_id, 12345678, test_handle);

  ESP_LOGI(TAG, "test end");
}
