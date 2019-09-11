#include "esp_err.h"
#include "esp_timer.h"

#include "rs232_lora_jxyl001.h"

// global define ==============================================================
static const char *TAG = "RS232_LORA_JXYL001";

// static func ================================================================
static void rs232_lora_jxyl001_print_data(Rs232_Lora_Jxyl001_Devs *devs)
{
  for (int i = 0; i < devs->dev_num; i++)
  {
    switch (devs->devs[i]->type)
    {
    case RS232_LORA_JXYL001_TYPE_YL102:
      ESP_LOGW(TAG, "addr:%12u type:yl102 updata:%lld votage:%2.2f fog:%d",
               devs->devs[i]->addr, devs->devs[i]->update_time,
               devs->devs[i]->yl102_votage, devs->devs[i]->yl102_fog);
      break;
    case RS232_LORA_JXYL001_TYPE_YL103:
      ESP_LOGW(
          TAG,
          "addr:%12u type:yl103 updata:%lld votage:%2.2f temp:%2.2f hum:%2.2f",
          devs->devs[i]->addr, devs->devs[i]->update_time,
          devs->devs[i]->yl103_votage, devs->devs[i]->yl103_temp,
          devs->devs[i]->yl103_hum);
      break;
    default:
      ESP_LOGE(TAG, "%4d %s get unknown sensor type", __LINE__, __func__);
    }
  }
}

static void rs232_lora_jxyl001_data_parse(Rs232_Lora_Jxyl001_Devs *devs,
                                          uint8_t *data, int data_size)
{
  uint32_t addr = 0;
  Rs232_Lora_Jxyl001_Type type = RS232_LORA_JXYL001_TYPE_UNKNOWN;

  if (data_size < 8)
  {
    ESP_LOGE(TAG, "%4d %s get unexcept data_size:%d", __LINE__, __func__,
             data_size);
    return;
  }

  if (data[0] != 0x5e)
  {
    ESP_LOGE(TAG, "%4d %s get unexcept data[0]:%2x", __LINE__, __func__,
             data[0]);
    return;
  }

  addr = ((uint32_t)data[1] * (1 << 24)) + ((uint32_t)data[2] * (1 << 16)) +
         ((uint32_t)data[3] * (1 << 8)) + (uint32_t)data[4];

  switch (data[5])
  {
  case 0xb0:
    type = RS232_LORA_JXYL001_TYPE_YL103;
    if (data[7] != 0x06)
    {
      ESP_LOGE(TAG, "%4d %s data_len not match yl103", __LINE__, __func__);
      return;
    }

    if (data_size != 16)
    {
      ESP_LOGE(TAG, "%4d %s data_size not match yl103", __LINE__, __func__);
      return;
    }
    break;
  case 0xb1:
    type = RS232_LORA_JXYL001_TYPE_YL102;
    if (data[7] != 0x02)
    {
      ESP_LOGE(TAG, "%4d %s data_len not match yl102", __LINE__, __func__);
      return;
    }

    if (data_size != 12)
    {
      ESP_LOGE(TAG, "%4d %s data_size not match yl102", __LINE__, __func__);
      return;
    }
    break;
  default:
    type = RS232_LORA_JXYL001_TYPE_UNKNOWN;
    ESP_LOGE(TAG, "%4d %s get unknown sensor type", __LINE__, __func__);
    return;
  }

  for (int i = 0; i < devs->dev_num; i++)
  {
    if (addr == devs->devs[i]->addr)
    {
      switch (type)
      {
      case RS232_LORA_JXYL001_TYPE_YL102:
        if (data[7] == 0x01)
        {
          devs->devs[i]->yl102_fog = false;
          devs->devs[i]->update_time = esp_timer_get_time() / 1000000;
        }
        else
        {
          devs->devs[i]->yl102_fog = true;
          devs->devs[i]->update_time = esp_timer_get_time() / 1000000;
        }
        devs->devs[i]->yl102_votage = data[9] / 10.0;
        break;
      case RS232_LORA_JXYL001_TYPE_YL103:
        devs->devs[i]->update_time = esp_timer_get_time() / 1000000;
        devs->devs[i]->yl103_votage = data[9] / 10.0;
        devs->devs[i]->yl103_temp =
            (((uint32_t)data[10] * (1 << 8)) + (uint32_t)data[11]) / 10.0;
        devs->devs[i]->yl103_hum =
            (((uint32_t)data[12] * (1 << 8)) + (uint32_t)data[13]) / 10.0;
        break;
      default:
        ESP_LOGE(TAG, "%4d %s get unknown sensor type", __LINE__, __func__);
        return;
      }
    }
  }
}

static void rs232_lora_jxyl001_loop(Rs232_Lora_Jxyl001_Devs *devs)
{
  while (1)
  {
    int read_size = 0;
    uint8_t *read_buf = NULL;

    read_buf = rs232_dev_read(devs->rs232_config, &read_size);
    if (read_size > 0)
    {
      ESP_LOGI(TAG, "%4d %s rs232_dev_read, size=%d", __LINE__, __func__,
               read_size);
      for (int i = 0; i < read_size; i++)
      {
        printf("%2x ", read_buf[i]);
      }
      printf("\n");

      rs232_lora_jxyl001_data_parse(devs, read_buf, read_size);
      rs232_lora_jxyl001_print_data(devs);
    }

    if (read_buf != NULL)
      free(read_buf);
  }
}

// global func ================================================================

Rs232_Lora_Jxyl001_t *rs232_lora_jxyl001_default_new(
    uint32_t addr, Rs232_Lora_Jxyl001_Type type)
{
  Rs232_Lora_Jxyl001_t *jxyl001_out = malloc(sizeof(Rs232_Lora_Jxyl001_t));

  jxyl001_out->addr = addr;
  jxyl001_out->type = type;
  jxyl001_out->update_time = 0;
  jxyl001_out->yl102_votage = 0;
  jxyl001_out->yl102_fog = false;
  jxyl001_out->yl103_votage = 0;
  jxyl001_out->yl103_temp = 0;
  jxyl001_out->yl103_hum = 0;

  return jxyl001_out;
}

Rs232_Lora_Jxyl001_Devs *rs232_lora_jxyl001_devs_default_new(int dev_num)
{
  Rs232_Lora_Jxyl001_Devs *devs = malloc(sizeof(Rs232_Lora_Jxyl001_Devs));

  devs->rs232_config = NULL;
  devs->dev_num = dev_num;
  devs->devs = malloc(dev_num * sizeof(Rs232_Lora_Jxyl001_t));

  for (int i = 0; i < dev_num; i++)
  {
    devs->devs[i] = NULL;
  }

  return devs;
}

esp_err_t rs232_lora_jxyl001_task(Rs232_Lora_Jxyl001_Devs *devs)
{
  esp_err_t err = ESP_OK;

  if (devs->rs232_config != NULL)
    rs232_dev_config_free(devs->rs232_config);

  devs->rs232_config = rs232_dev_default_new();

  err = rs232_dev_init(devs->rs232_config);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s rs232_dev_init failed", __LINE__, __func__);
    goto EXIT;
  }

  xTaskCreate((TaskFunction_t)rs232_lora_jxyl001_loop, "JXYL001_TASK", 8 * 1024,
              devs, 10, NULL);

EXIT:
  if (err != ESP_OK)
    rs232_dev_config_free(devs->rs232_config);

  return err;
}
