#include "stdint.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "modbus_ld100.h"

// global define ==============================================================
static const char *TAG = "modbus_ld100_example";

UCHAR RS485_PORT = 2;
ULONG RS485_BAUD = 9600;
eMBParity RS485_PARITY = MB_PAR_NONE;
int TX_PIN = 23;
int RX_PIN = 22;
int EN_PIN = 18;

// main func ==================================================================
void app_main()
{
  eMBMasterReqErrCode cmd_ret = 0;
  eMBErrorCode emb_ret = 0;
  struct RetMsg_t cmd_ret_payload;

  ESP_LOGI(TAG, "test begin");

  emb_ret = modbus_init(RS485_PORT, RS485_BAUD, RS485_PARITY, TX_PIN, RX_PIN, EN_PIN);
  if (emb_ret != 0)
  {
    ESP_LOGE(TAG, "%4d modbus_init failed", __LINE__);
    return;
  }

  mt_modbus_task();

  vTaskDelay(2000 / portTICK_RATE_MS);
  // test addr
  cmd_ret = modbus_ld100_sync_Cmd_04(01, 00, 01, &cmd_ret_payload);
  if (cmd_ret != MB_MRE_NO_ERR)
  {
    ESP_LOGE(TAG, "%4d modbus_ld100_sync_Cmd_04 failed", __LINE__);
  }
  ESP_LOGI(TAG, "%4d test addr cmd:%d len:%d buf:%2x%2x", __LINE__,
           cmd_ret_payload.recvCmd, cmd_ret_payload.retLen,
           (unsigned int)(cmd_ret_payload.retBuf[0]),
           (unsigned int)(cmd_ret_payload.retBuf[1]));

  vTaskDelay(2000 / portTICK_RATE_MS);
  // test relay status
  cmd_ret = modbus_ld100_sync_Cmd_04(01, 03, 01, &cmd_ret_payload);
  if (cmd_ret != MB_MRE_NO_ERR)
  {
    ESP_LOGE(TAG, "%4d modbus_ld100_sync_Cmd_04 failed", __LINE__);
  }
  ESP_LOGI(TAG, "%4d test relay status cmd:%d len:%d buf:%2x%2x", __LINE__,
           cmd_ret_payload.recvCmd, cmd_ret_payload.retLen,
           (unsigned int)(cmd_ret_payload.retBuf[0]),
           (unsigned int)(cmd_ret_payload.retBuf[1]));

  vTaskDelay(2000 / portTICK_RATE_MS);
  // test set relay on
  cmd_ret = modbus_ld100_sync_Cmd_06(01, 03, 01, &cmd_ret_payload);
  if (cmd_ret != MB_MRE_NO_ERR)
  {
    ESP_LOGE(TAG, "%4d modbus_ld100_sync_Cmd_06 failed", __LINE__);
  }
  ESP_LOGI(TAG, "%4d test relay status cmd:%d len:%d buf:%2x%2x", __LINE__,
           cmd_ret_payload.recvCmd, cmd_ret_payload.retLen,
           (unsigned int)(cmd_ret_payload.retBuf[0]),
           (unsigned int)(cmd_ret_payload.retBuf[1]));

  vTaskDelay(2000 / portTICK_RATE_MS);
  // test set relay off
  cmd_ret = modbus_ld100_sync_Cmd_06(01, 03, 00, &cmd_ret_payload);
  if (cmd_ret != MB_MRE_NO_ERR)
  {
    ESP_LOGE(TAG, "%4d modbus_ld100_sync_Cmd_06 failed", __LINE__);
  }
  ESP_LOGI(TAG, "%4d test relay status cmd:%d len:%d buf:%2x%2x", __LINE__,
           cmd_ret_payload.recvCmd, cmd_ret_payload.retLen,
           (unsigned int)(cmd_ret_payload.retBuf[0]),
           (unsigned int)(cmd_ret_payload.retBuf[1]));

  ESP_LOGI(TAG, "test end");

  while (1)
  {
    vTaskDelay(200 / portTICK_RATE_MS);
    // test status
    cmd_ret = modbus_ld100_sync_Cmd_04(01, 01, 01, &cmd_ret_payload);
    if (cmd_ret != MB_MRE_NO_ERR)
    {
      ESP_LOGE(TAG, "%4d modbus_ld100_sync_Cmd_04 failed", __LINE__);
    }
    ESP_LOGI(TAG, "%4d test status cmd:%d len:%d buf:%2x%2x", __LINE__,
             cmd_ret_payload.recvCmd, cmd_ret_payload.retLen,
             (unsigned int)(cmd_ret_payload.retBuf[0]),
             (unsigned int)(cmd_ret_payload.retBuf[1]));
  }
}