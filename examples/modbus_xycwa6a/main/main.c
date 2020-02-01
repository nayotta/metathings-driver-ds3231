#include "stdint.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "modbus_xycwa6a.h"

// global define ==============================================================
static const char *TAG = "modbus_xycwa6a_example";

UCHAR RS485_PORT = 2;
ULONG RS485_BAUD = 9600;
eMBParity RS485_PARITY = MB_PAR_NONE;
int TX_PIN = 13;
int RX_PIN = 15;
int EN_PIN = 05;

int ADDR = 1;

// test func =================================================================
void test_xycwa6a_get_temps() {
  esp_err_t err = ESP_OK;
  double temp1 = 0;
  double temp2 = 0;
  double temp3 = 0;
  double temp4 = 0;
  double temp5 = 0;
  double temp6 = 0;

  err = mt_xycwa6a_get_temps(ADDR, &temp1, &temp2, &temp3, &temp4, &temp5,
                             &temp6);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed code:%d", __LINE__, __func__, err);
    return;
  }

  return;
}

// main func ==================================================================
void app_main() {
  eMBErrorCode emb_ret = 0;

  ESP_LOGI(TAG, "test begin");

  emb_ret = modbus_xycwa6a_init(RS485_PORT, RS485_BAUD, RS485_PARITY, TX_PIN,
                                RX_PIN, EN_PIN);
  if (emb_ret != 0) {
    ESP_LOGE(TAG, "%4d modbus_init failed", __LINE__);
    return;
  }

  mt_modbus_xycwa6a_task();

  ESP_LOGI(TAG, "test end");

  while (1) {
    test_xycwa6a_get_temps();
    vTaskDelay(2000 / portTICK_RATE_MS);
  }
}
