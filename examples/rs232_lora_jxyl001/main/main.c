#include "esp_log.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "rs232_dev.h"
#include "rs232_lora_jxyl001.h"

// global define ==============================================================
static const char *TAG = "example-rs232-lora-jxyl001";

int ADDR1 = 1;
Rs232_Lora_Jxyl001_Type TYPE1 = RS232_LORA_JXYL001_TYPE_YL102;
int ADDR2 = 2;
Rs232_Lora_Jxyl001_Type TYPE2 = RS232_LORA_JXYL001_TYPE_YL103;

// gloabal func ===============================================================
void app_main() {
  esp_err_t err = ESP_OK;
  Rs232_Lora_Jxyl001_Devs *devs = NULL;

  ESP_LOGI(TAG, "test begin");

  devs = rs232_lora_jxyl001_devs_default_new(2);
  devs->devs[0] = rs232_lora_jxyl001_default_new(ADDR1, TYPE1);
  devs->devs[1] = rs232_lora_jxyl001_default_new(ADDR2, TYPE2);

  err = rs232_lora_jxyl001_task(devs);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_jxyl001_task failed", __LINE__, __func__);
    return;
  }

  ESP_LOGI(TAG, "test end");
}
