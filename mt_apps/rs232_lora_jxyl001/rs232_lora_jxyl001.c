#include "esp_err.h"
#include "esp_timer.h"

#include "mt_nvs_storage.h"
#include "rs232_lora_jxyl001.h"

// global define ==============================================================
static const char *TAG = "RS232_LORA_JXYL001";

Rs232_Lora_Jxyl001_Devs *DEVS = NULL;

// static func ================================================================
static void rs232_lora_jxyl001_free_devs() {
  if (DEVS == NULL)
    return;

  if (DEVS->rs232_config != NULL)
    rs232_dev_config_free(DEVS->rs232_config);

  for (int i = 0; i < DEVS->fog_num; i++) {
    if (DEVS->fogs[i] != NULL)
      free(DEVS->fogs[i]);
  }

  for (int i = 0; i < DEVS->temp_num; i++) {
    if (DEVS->temps[i] != NULL)
      free(DEVS->temps[i]);
  }

  if (DEVS != NULL)
    free(DEVS);
}

static void rs232_lora_jxyl001_new_devs() {
  rs232_lora_jxyl001_free_devs();

  DEVS = malloc(sizeof(Rs232_Lora_Jxyl001_Devs));
  DEVS->rs232_config = NULL;
  DEVS->fog_num = 0;
  DEVS->fogs = NULL;
  DEVS->temp_num = 0;
  DEVS->temps = NULL;
}

static rs232_lora_jxyl001_fog *rs232_lora_jxyl001_new_fog() {
  rs232_lora_jxyl001_fog *fog = NULL;

  fog = malloc(sizeof(rs232_lora_jxyl001_fog));
  fog->addr = 0;
  fog->update_time = 0;
  fog->fog = false;
  fog->votage = 0;

  return fog;
}

static rs232_lora_jxyl001_temp *rs232_lora_jxyl001_new_temp() {
  rs232_lora_jxyl001_temp *temp = NULL;

  temp = malloc(sizeof(rs232_lora_jxyl001_temp));
  temp->addr = 0;
  temp->update_time = 0;
  temp->temp = 0;
  temp->hum = 0;
  temp->votage = 0;

  return temp;
}

static void rs232_lora_jxyl001_print_data() {
  for (int i = 0; i < DEVS->fog_num; i++) {
    ESP_LOGW(TAG, "addr:%12u type:fog updata:%lld votage:%2.2f fog:%d",
             DEVS->fogs[i]->addr, DEVS->fogs[i]->update_time,
             DEVS->fogs[i]->votage, DEVS->fogs[i]->fog);
  }

  for (int i = 0; i < DEVS->temp_num; i++) {
    ESP_LOGW(
        TAG,
        "addr:%12u type:temp updata:%lld votage:%2.2f temp:%2.2f hum:%2.2f",
        DEVS->temps[i]->addr, DEVS->temps[i]->update_time,
        DEVS->temps[i]->votage, DEVS->temps[i]->temp, DEVS->temps[i]->hum);
  }
}

static double rs232_lora_jxyl001_format_votage(double votage_in) {
  double ret = 0;
  double max = 4.2;
  double min = 3.3;

  if (votage_in > max)
    ret = 100.0;

  if (votage_in < min)
    ret = 0.0;

  if (votage_in >= min && votage_in <= max) {
    ret = (votage_in - min) / (max - min) * 100.0;
  }

  return ret;
}

static void rs232_lora_jxyl001_data_parse(uint8_t *data, int data_size) {
  uint32_t addr = 0;

  if (data_size < 8) {
    ESP_LOGE(TAG, "%4d %s get unexcept data_size:%d", __LINE__, __func__,
             data_size);
    return;
  }

  if (data[0] != 0x5e) {
    ESP_LOGE(TAG, "%4d %s get unexcept data[0]:%2x", __LINE__, __func__,
             data[0]);
    return;
  }

  addr = ((uint32_t)data[1] * (1 << 24)) + ((uint32_t)data[2] * (1 << 16)) +
         ((uint32_t)data[3] * (1 << 8)) + (uint32_t)data[4];

  switch (data[5]) {
  case 0xb0:
    if (data[7] != 0x06) {
      ESP_LOGE(TAG, "%4d %s data_len not match yl103", __LINE__, __func__);
      return;
    }

    if (data_size != 16) {
      ESP_LOGE(TAG, "%4d %s data_size not match yl103", __LINE__, __func__);
      return;
    }
    break;
  case 0xb1:
    if (data[7] != 0x02) {
      ESP_LOGE(TAG, "%4d %s data_len not match yl102", __LINE__, __func__);
      return;
    }

    if (data_size != 12) {
      ESP_LOGE(TAG, "%4d %s data_size not match yl102", __LINE__, __func__);
      return;
    }
    break;
  default:
    ESP_LOGE(TAG, "%4d %s get unknown sensor type", __LINE__, __func__);
    return;
  }

  // process fog
  for (int i = 0; i < DEVS->fog_num; i++) {
    if (addr == DEVS->fogs[i]->addr) {
      if (data[7] == 0x01) {
        DEVS->fogs[i]->fog = false;
        DEVS->fogs[i]->update_time = esp_timer_get_time() / 1000000;
      } else {
        DEVS->fogs[i]->fog = true;
        DEVS->fogs[i]->update_time = esp_timer_get_time() / 1000000;
      }
      DEVS->fogs[i]->votage = rs232_lora_jxyl001_format_votage(data[9] / 10.0);
    }
  }

  // process temp and hum
  for (int i = 0; i < DEVS->temp_num; i++) {
    if (addr == DEVS->temps[i]->addr) {
      DEVS->temps[i]->update_time = esp_timer_get_time() / 1000000;
      DEVS->temps[i]->votage = rs232_lora_jxyl001_format_votage(data[9] / 10.0);
      DEVS->temps[i]->temp =
          (((uint32_t)data[10] * (1 << 8)) + (uint32_t)data[11]) / 10.0;
      DEVS->temps[i]->hum =
          (((uint32_t)data[12] * (1 << 8)) + (uint32_t)data[13]) / 10.0;
    }
  }
}

static void rs232_lora_jxyl001_loop() {
  while (1) {
    int read_size = 0;
    uint8_t *read_buf = NULL;

    read_buf = rs232_dev_read(DEVS->rs232_config, &read_size);
    if (read_size > 0) {
      ESP_LOGI(TAG, "%4d %s rs232_dev_read, size=%d", __LINE__, __func__,
               read_size);
      for (int i = 0; i < read_size; i++) {
        printf("%2x ", read_buf[i]);
      }
      printf("\n");

      rs232_lora_jxyl001_data_parse(read_buf, read_size);
      rs232_lora_jxyl001_print_data();
    }

    if (read_buf != NULL)
      free(read_buf);
  }

  vTaskDelete(NULL);
}

// global func ================================================================
esp_err_t rs232_lora_jxyl001_task() {
  esp_err_t err = ESP_OK;
  char key[16] = "";

  rs232_lora_jxyl001_new_devs();

  DEVS->rs232_config = rs232_dev_default_new();

  err = rs232_dev_init(DEVS->rs232_config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_dev_init failed", __LINE__, __func__);
    goto EXIT;
  }

  if (mt_nvs_read_int32_config("fog_num", &DEVS->fog_num) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_read_int32_config fog_num failed", __LINE__,
             __func__);
    goto EXIT;
  }

  DEVS->fogs = (rs232_lora_jxyl001_fog **)malloc(
      DEVS->fog_num * sizeof(rs232_lora_jxyl001_fog *));
  for (int i = 0; i < DEVS->fog_num; i++) {
    DEVS->fogs[i] = rs232_lora_jxyl001_new_fog();
    sprintf(key, "fog_addr_%d", i + 1);
    if (mt_nvs_read_int32_config(key, &DEVS->fogs[i]->addr) == false) {
      ESP_LOGE(TAG, "%4d %s mt_nvs_read_int32_config %s failed", __LINE__,
               __func__, key);
      goto EXIT;
    }
    ESP_LOGI(TAG, "%4d %s fog index:%d addr:%d", __LINE__, __func__, i + 1,
             DEVS->fogs[i]->addr);
  }

  if (mt_nvs_read_int32_config("temp_num", &DEVS->temp_num) == false) {
    ESP_LOGE(TAG, "%4d %s mt_nvs_read_int32_config temp_num failed", __LINE__,
             __func__);
    goto EXIT;
  }

  DEVS->temps = (rs232_lora_jxyl001_temp **)malloc(
      DEVS->temp_num * sizeof(rs232_lora_jxyl001_temp *));
  for (int i = 0; i < DEVS->temp_num; i++) {
    DEVS->temps[i] = rs232_lora_jxyl001_new_temp();
    sprintf(key, "temp_addr_%d", i + 1);
    if (mt_nvs_read_int32_config(key, &DEVS->temps[i]->addr) == false) {
      ESP_LOGE(TAG, "%4d %s mt_nvs_read_int32_config %s failed", __LINE__,
               __func__, key);
      goto EXIT;
    }
    ESP_LOGI(TAG, "%4d %s temp index:%d addr:%d", __LINE__, __func__, i + 1,
             DEVS->temps[i]->addr);
  }

  xTaskCreate(rs232_lora_jxyl001_loop, "JXYL001_TASK", 8 * 1024, NULL, 10,
              NULL);

EXIT:
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
    rs232_lora_jxyl001_free_devs();
  }

  return err;
}

Rs232_Lora_Jxyl001_Devs *rs232_lora_jxyl001_get_devs() { return DEVS; }
