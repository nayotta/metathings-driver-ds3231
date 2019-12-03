#include "esp_log.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "rs232_PRS3342.h"
#include "rs232_dev.h"

// global define ==============================================================

static const char *TAG = "example-rs232-PRS3342";

// gloabal func ===============================================================

void app_main() {
  esp_err_t err = ESP_OK;
  rs232_PRS3342_t *prs3342 = malloc(sizeof(rs232_PRS3342_t));
  prs3342->rs232_config = NULL;
  prs3342->datas = NULL;

  ESP_LOGI(TAG, "test begin");

  prs3342->rs232_config = rs232_dev_default_new();

  prs3342->data_num = 31;
  prs3342->datas = malloc(prs3342->data_num * sizeof(rs232_PRS3342_data_t *));
  for (int i = 0; i < prs3342->data_num; i++) {
    prs3342->datas[i] = malloc(sizeof(rs232_PRS3342_data_t));
  }

  int offset = 0;
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4003, 32.5, "Ua");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4004, 32.5, "Ub");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4005, 32.5, "Uc");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4010, 1250, "P1");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4011, 1250, "Q1");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4012, 4000, "Ia1");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4013, 4000, "Ib1");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4014, 4000, "Ic1");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4015, 4000, "I01");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4016, 100, "COS1");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4017, 1250, "P2");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4018, 1250, "Q2");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4019, 4000, "Ia2");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x401a, 4000, "Ib2");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x401b, 4000, "Ic2");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x401c, 4000, "I02");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x401d, 100, "COS2");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x401e, 1250, "P3");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x401f, 1250, "Q3");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4020, 4000, "Ia3");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4021, 4000, "Ib3");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4022, 4000, "Ic3");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4023, 4000, "I03");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4024, 100, "COS3");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4025, 1250, "P4");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4026, 1250, "Q4");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4027, 4000, "Ia4");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4028, 4000, "Ib4");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4029, 4000, "Ic4");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x402a, 4000, "I04");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x402b, 100, "COS4");

  err = rs232_PRS3342_task(prs3342);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_PRS3342_task failed", __LINE__, __func__);
    return;
  }

  ESP_LOGI(TAG, "test end");
}
