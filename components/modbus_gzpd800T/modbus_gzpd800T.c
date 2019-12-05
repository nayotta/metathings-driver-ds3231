#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "mt_mbfunc.h"
#include "mt_mbtask.h"
#include "mt_port.h"

#include "modbus_gzpd800T.h"
#include "mt_mbtask.h"

// global define ==============================================================
static const char *TAG = "MT_MODBUS_gzpd800T";
#define GZPD800T_DEAULT_ADDR 01
#define GZPD800T_READ_DATA 03
#define GZPD800T_READ_WARN 01

#define GZPD800T_DATA_ADDR 0x0300
#define GZPD800T_DATA_SIZE 4 * 6
#define GZPD800T_WARN_SIZE 1

// global func ================================================================
esp_err_t mt_gzpd800T_get_4ch_data(gzpd800T_4ch_data_t *data)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  err = modbus_sync_Cmd_03(GZPD800T_DEAULT_ADDR, GZPD800T_DATA_ADDR,
                           GZPD800T_DATA_SIZE, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    return err;
  }

  if (cmd_ret_payload.recvCmd != GZPD800T_READ_DATA)
  {
    ESP_LOGE(TAG, "%4d %s  get error ret cmd:%d", __LINE__, __func__,
             cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != 2 * GZPD800T_DATA_SIZE)
  {
    ESP_LOGE(TAG, "%4d %s get error ret size:%d", __LINE__, __func__,
             cmd_ret_payload.retLen);
    return ESP_ERR_INVALID_RESPONSE;
  }

  // for (int i = 0; i < 2 * GZPD800T_DATA_SIZE; i++)
  //  printf("%2x ", cmd_ret_payload.retBuf[i]);
  // printf("\n");

  data->amp1 = (uint32_t)cmd_ret_payload.retBuf[0] << 24 |
               (uint32_t)cmd_ret_payload.retBuf[1] << 16 |
               (uint32_t)cmd_ret_payload.retBuf[2] << 8 |
               (uint32_t)cmd_ret_payload.retBuf[3];

  data->freq1 = (uint32_t)cmd_ret_payload.retBuf[4] << 24 |
                (uint32_t)cmd_ret_payload.retBuf[5] << 16 |
                (uint32_t)cmd_ret_payload.retBuf[6] << 8 |
                (uint32_t)cmd_ret_payload.retBuf[7];

  data->power1 = (uint32_t)cmd_ret_payload.retBuf[8] << 24 |
                 (uint32_t)cmd_ret_payload.retBuf[9] << 16 |
                 (uint32_t)cmd_ret_payload.retBuf[10] << 8 |
                 (uint32_t)cmd_ret_payload.retBuf[11];

  data->amp2 = (uint32_t)cmd_ret_payload.retBuf[12] << 24 |
               (uint32_t)cmd_ret_payload.retBuf[13] << 16 |
               (uint32_t)cmd_ret_payload.retBuf[14] << 8 |
               (uint32_t)cmd_ret_payload.retBuf[15];

  data->freq2 = (uint32_t)cmd_ret_payload.retBuf[16] << 24 |
                (uint32_t)cmd_ret_payload.retBuf[17] << 16 |
                (uint32_t)cmd_ret_payload.retBuf[18] << 8 |
                (uint32_t)cmd_ret_payload.retBuf[19];

  data->power2 = (uint32_t)cmd_ret_payload.retBuf[20] << 24 |
                 (uint32_t)cmd_ret_payload.retBuf[21] << 16 |
                 (uint32_t)cmd_ret_payload.retBuf[22] << 8 |
                 (uint32_t)cmd_ret_payload.retBuf[23];

  data->amp3 = (uint32_t)cmd_ret_payload.retBuf[24] << 24 |
               (uint32_t)cmd_ret_payload.retBuf[25] << 16 |
               (uint32_t)cmd_ret_payload.retBuf[26] << 8 |
               (uint32_t)cmd_ret_payload.retBuf[27];

  data->freq3 = (uint32_t)cmd_ret_payload.retBuf[28] << 24 |
                (uint32_t)cmd_ret_payload.retBuf[29] << 16 |
                (uint32_t)cmd_ret_payload.retBuf[30] << 8 |
                (uint32_t)cmd_ret_payload.retBuf[31];

  data->power3 = (uint32_t)cmd_ret_payload.retBuf[32] << 24 |
                 (uint32_t)cmd_ret_payload.retBuf[33] << 16 |
                 (uint32_t)cmd_ret_payload.retBuf[34] << 8 |
                 (uint32_t)cmd_ret_payload.retBuf[35];

  data->amp4 = (uint32_t)cmd_ret_payload.retBuf[36] << 24 |
               (uint32_t)cmd_ret_payload.retBuf[37] << 16 |
               (uint32_t)cmd_ret_payload.retBuf[38] << 8 |
               (uint32_t)cmd_ret_payload.retBuf[39];

  data->freq4 = (uint32_t)cmd_ret_payload.retBuf[40] << 24 |
                (uint32_t)cmd_ret_payload.retBuf[41] << 16 |
                (uint32_t)cmd_ret_payload.retBuf[42] << 8 |
                (uint32_t)cmd_ret_payload.retBuf[43];

  data->power4 = (uint32_t)cmd_ret_payload.retBuf[44] << 24 |
                 (uint32_t)cmd_ret_payload.retBuf[45] << 16 |
                 (uint32_t)cmd_ret_payload.retBuf[46] << 8 |
                 (uint32_t)cmd_ret_payload.retBuf[47];

  /*ESP_LOGW(TAG,
           "%4d %s \n"
           "amp1:%u freq1:%u power1:%u\n"
           "amp2:%u freq2:%u power2:%u\n"
           "amp3:%u freq3:%u power3:%u\n"
           "amp4:%u freq4:%u power4:%u\n",
           __LINE__, __func__, data->amp1, data->freq1, data->power1,
           data->amp2, data->freq2, data->power2, data->amp3, data->freq3,
           data->power3, data->amp4, data->freq4, data->power4);*/

  return ESP_OK;
}

esp_err_t mt_gzpd800T_get_warn(int addr, bool *warn)
{
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;
  USHORT addr_ushort = addr;

  err = modbus_sync_Cmd_01(GZPD800T_DEAULT_ADDR, addr_ushort,
                           GZPD800T_WARN_SIZE, &cmd_ret_payload);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    return err;
  }

  if (cmd_ret_payload.recvCmd != GZPD800T_READ_WARN)
  {
    ESP_LOGE(TAG, "%4d %s  get error ret cmd:%d", __LINE__, __func__,
             cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != GZPD800T_WARN_SIZE)
  {
    ESP_LOGE(TAG, "%4d %s get error ret size:%d", __LINE__, __func__,
             cmd_ret_payload.retLen);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retBuf[0] == 0)
  {
    *warn = false;
  }
  else
  {
    *warn = true;
  }

  //ESP_LOGW(TAG, "%4d %s get warn:%d", __LINE__, __func__, *warn);

  return ESP_OK;
}

esp_err_t mt_gzpd800T_init(int tx_pin, int rx_pin, int en_pin)
{
  eMBErrorCode emb_ret = 0;
  UCHAR RS485_PORT = 2;
  ULONG RS485_BAUD = 9600;
  eMBParity RS485_PARITY = MB_PAR_NONE;

  emb_ret =
      modbus_init(RS485_PORT, RS485_BAUD, RS485_PARITY, tx_pin, rx_pin, en_pin);
  if (emb_ret != 0)
  {
    ESP_LOGE(TAG, "%4d %s modbus_init failed", __LINE__, __func__);
    return emb_ret;
  }

  mt_vMBMaster_set_T35_interval(125);
  mt_modbus_task();

  return ESP_OK;
}
