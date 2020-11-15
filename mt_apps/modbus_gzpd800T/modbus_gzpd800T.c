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

static const char *TAG = "MODBUS_gzpd800T";
#define GZPD800T_DEAULT_ADDR 01
#define GZPD800T_READ_DATA 03
#define GZPD800T_READ_WARN 01

#define GZPD800T_DATA_ADDR 0x0300
#define GZPD800T_4CH_DATA_SIZE 4 * 6
#define GZPD800T_8CH_DATA_SIZE 8 * 6
#define GZPD800T_WARN_SIZE 1

// global func ================================================================

esp_err_t modbus_gzpd800T_get_4ch_data(gzpd800T_4ch_data_t *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  err = modbus_sync_Cmd_03(GZPD800T_DEAULT_ADDR, GZPD800T_DATA_ADDR,
                           GZPD800T_4CH_DATA_SIZE, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    return err;
  }

  if (cmd_ret_payload.recvCmd != GZPD800T_READ_DATA) {
    ESP_LOGE(TAG, "%4d %s  get error ret cmd:%d", __LINE__, __func__,
             cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != 2 * GZPD800T_4CH_DATA_SIZE) {
    ESP_LOGE(TAG, "%4d %s get error ret size:%d", __LINE__, __func__,
             cmd_ret_payload.retLen);
    return ESP_ERR_INVALID_RESPONSE;
  }

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

esp_err_t modbus_gzpd800T_get_8ch_data(gzpd800T_8ch_data_t *data) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;

  err = modbus_sync_Cmd_03(GZPD800T_DEAULT_ADDR, GZPD800T_DATA_ADDR,
                           GZPD800T_8CH_DATA_SIZE, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    return err;
  }

  if (cmd_ret_payload.recvCmd != GZPD800T_READ_DATA) {
    ESP_LOGE(TAG, "%4d %s  get error ret cmd:%d", __LINE__, __func__,
             cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != 2 * GZPD800T_8CH_DATA_SIZE) {
    ESP_LOGE(TAG, "%4d %s get error ret size:%d", __LINE__, __func__,
             cmd_ret_payload.retLen);
    return ESP_ERR_INVALID_RESPONSE;
  }

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

  data->amp5 = (uint32_t)cmd_ret_payload.retBuf[48] << 24 |
               (uint32_t)cmd_ret_payload.retBuf[49] << 16 |
               (uint32_t)cmd_ret_payload.retBuf[50] << 8 |
               (uint32_t)cmd_ret_payload.retBuf[51];

  data->freq5 = (uint32_t)cmd_ret_payload.retBuf[52] << 24 |
                (uint32_t)cmd_ret_payload.retBuf[53] << 16 |
                (uint32_t)cmd_ret_payload.retBuf[54] << 8 |
                (uint32_t)cmd_ret_payload.retBuf[55];

  data->power5 = (uint32_t)cmd_ret_payload.retBuf[56] << 24 |
                 (uint32_t)cmd_ret_payload.retBuf[57] << 16 |
                 (uint32_t)cmd_ret_payload.retBuf[58] << 8 |
                 (uint32_t)cmd_ret_payload.retBuf[59];

  data->amp6 = (uint32_t)cmd_ret_payload.retBuf[60] << 24 |
               (uint32_t)cmd_ret_payload.retBuf[61] << 16 |
               (uint32_t)cmd_ret_payload.retBuf[62] << 8 |
               (uint32_t)cmd_ret_payload.retBuf[63];

  data->freq6 = (uint32_t)cmd_ret_payload.retBuf[64] << 24 |
                (uint32_t)cmd_ret_payload.retBuf[65] << 16 |
                (uint32_t)cmd_ret_payload.retBuf[66] << 8 |
                (uint32_t)cmd_ret_payload.retBuf[67];

  data->power6 = (uint32_t)cmd_ret_payload.retBuf[68] << 24 |
                 (uint32_t)cmd_ret_payload.retBuf[69] << 16 |
                 (uint32_t)cmd_ret_payload.retBuf[70] << 8 |
                 (uint32_t)cmd_ret_payload.retBuf[71];

  data->amp7 = (uint32_t)cmd_ret_payload.retBuf[72] << 24 |
               (uint32_t)cmd_ret_payload.retBuf[73] << 16 |
               (uint32_t)cmd_ret_payload.retBuf[74] << 8 |
               (uint32_t)cmd_ret_payload.retBuf[75];

  data->freq7 = (uint32_t)cmd_ret_payload.retBuf[76] << 24 |
                (uint32_t)cmd_ret_payload.retBuf[77] << 16 |
                (uint32_t)cmd_ret_payload.retBuf[78] << 8 |
                (uint32_t)cmd_ret_payload.retBuf[79];

  data->power7 = (uint32_t)cmd_ret_payload.retBuf[80] << 24 |
                 (uint32_t)cmd_ret_payload.retBuf[81] << 16 |
                 (uint32_t)cmd_ret_payload.retBuf[82] << 8 |
                 (uint32_t)cmd_ret_payload.retBuf[83];

  data->amp8 = (uint32_t)cmd_ret_payload.retBuf[84] << 24 |
               (uint32_t)cmd_ret_payload.retBuf[85] << 16 |
               (uint32_t)cmd_ret_payload.retBuf[86] << 8 |
               (uint32_t)cmd_ret_payload.retBuf[87];

  data->freq8 = (uint32_t)cmd_ret_payload.retBuf[88] << 24 |
                (uint32_t)cmd_ret_payload.retBuf[89] << 16 |
                (uint32_t)cmd_ret_payload.retBuf[90] << 8 |
                (uint32_t)cmd_ret_payload.retBuf[91];

  data->power8 = (uint32_t)cmd_ret_payload.retBuf[92] << 24 |
                 (uint32_t)cmd_ret_payload.retBuf[93] << 16 |
                 (uint32_t)cmd_ret_payload.retBuf[94] << 8 |
                 (uint32_t)cmd_ret_payload.retBuf[95];

  ESP_LOGW(TAG,
           "%4d %s \n"
           "amp1:%u freq1:%u power1:%u\n"
           "amp2:%u freq2:%u power2:%u\n"
           "amp3:%u freq3:%u power3:%u\n"
           "amp4:%u freq4:%u power4:%u\n"
           "amp5:%u freq5:%u power5:%u\n"
           "amp6:%u freq6:%u power6:%u\n"
           "amp7:%u freq7:%u power7:%u\n"
           "amp8:%u freq8:%u power8:%u\n",
           __LINE__, __func__, data->amp1, data->freq1, data->power1,
           data->amp2, data->freq2, data->power2, data->amp3, data->freq3,
           data->power3, data->amp4, data->freq4, data->power4, data->amp5,
           data->freq5, data->power5, data->amp6, data->freq6, data->power6,
           data->amp7, data->freq7, data->power7, data->amp8, data->freq8,
           data->power8);

  return ESP_OK;
}

esp_err_t modbus_gzpd800T_get_warn(int addr, bool *warn) {
  esp_err_t err = ESP_OK;
  struct RetMsg_t cmd_ret_payload;
  USHORT addr_ushort = addr;

  err = modbus_sync_Cmd_01(GZPD800T_DEAULT_ADDR, addr_ushort,
                           GZPD800T_WARN_SIZE, &cmd_ret_payload);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    return err;
  }

  if (cmd_ret_payload.recvCmd != GZPD800T_READ_WARN) {
    ESP_LOGE(TAG, "%4d %s  get error ret cmd:%d", __LINE__, __func__,
             cmd_ret_payload.recvCmd);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retLen != GZPD800T_WARN_SIZE) {
    ESP_LOGE(TAG, "%4d %s get error ret size:%d", __LINE__, __func__,
             cmd_ret_payload.retLen);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cmd_ret_payload.retBuf[0] == 0) {
    *warn = false;
  } else {
    *warn = true;
  }

  // ESP_LOGW(TAG, "%4d %s get warn:%d", __LINE__, __func__, *warn);

  return ESP_OK;
}

esp_err_t modbus_gzpd800T_init(uint8_t port, int tx_pin, int rx_pin,
                               int en_pin) {
  eMBErrorCode emb_ret = 0;
  UCHAR RS485_PORT = 2;
  ULONG RS485_BAUD = 9600;
  eMBParity RS485_PARITY = MB_PAR_NONE;

  emb_ret =
      modbus_init(RS485_PORT, RS485_BAUD, RS485_PARITY, tx_pin, rx_pin, en_pin);
  if (emb_ret != 0) {
    ESP_LOGE(TAG, "%4d %s modbus_init failed", __LINE__, __func__);
    return emb_ret;
  }

  mt_vMBMaster_set_T35_interval(125);
  mt_modbus_task();

  return ESP_OK;
}
