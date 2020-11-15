#include "esp_log.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "mt_memory_manage.h"

#include "rs232_PRS3342.h"
#include "rs232_dev.h"
#include "rs232_lora_ebyte.h"

// global define ==============================================================

static const char *TAG = "example-rs232-PRS3342";

static int RS232_UART_NUM = 1;
static int RS232_RX_PIN = 15;
static int RS232_TX_PIN = 13;

static int LORA_UART_NUM = 2;
static int LORA_BAUD_RATE = 9600;
static int LORA_RX_PIN = 4;
static int LORA_TX_PIN = 16;

// gloabal func ===============================================================

void app_main() {
  esp_err_t err = ESP_OK;
  rs232_PRS3342_t *prs3342 = malloc(sizeof(rs232_PRS3342_t));
  prs3342->rs232_config = NULL;
  prs3342->datas = NULL;

  ESP_LOGI(TAG, "test begin");

  mt_memory_manage_task(true);

  err = rs232_lora_ebyte_init(LORA_UART_NUM, LORA_RX_PIN, LORA_TX_PIN,
                              LORA_BAUD_RATE);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_init failed", __LINE__, __func__);
    return;
  }

  prs3342->rs232_config = rs232_dev_default_new();
  prs3342->rs232_config->uart_num = RS232_UART_NUM;
  prs3342->rs232_config->rx_pin = RS232_RX_PIN;
  prs3342->rs232_config->tx_pin = RS232_TX_PIN;
  prs3342->rs232_config->timeout = 200;
  prs3342->data_num = 90;
  prs3342->datas = malloc(prs3342->data_num * sizeof(rs232_PRS3342_data_t *));
  for (int i = 0; i < prs3342->data_num; i++) {
    prs3342->datas[i] = malloc(sizeof(rs232_PRS3342_data_t));
  }

  int offset = 0;
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4004, 32768 * 0.001,
                          "Uab1");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4005, 32768 * 0.001,
                          "Ubc1");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4006, 32768 * 0.001,
                          "Uac1");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4008, 32768 * 0.12,
                          "Ia1");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4009, 32768 * 0.12,
                          "Ib1");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x400a, 32768 * 0.12,
                          "Ic1");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x400b, 32768 * 0.02,
                          "I01");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x400c, 32768, "P1");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x400e, 32768 * 0.01,
                          "COS1");

  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4018, 32768 * 0.001,
                          "Uab2");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4019, 32768 * 0.001,
                          "Ubc2");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x401a, 32768 * 0.001,
                          "Uac2");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x401c, 32768 * 0.12,
                          "Ia2");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x401d, 32768 * 0.12,
                          "Ib2");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x401e, 32768 * 0.12,
                          "Ic2");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x401f, 32768 * 0.02,
                          "I02");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4020, 32768, "P2");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4022, 32768 * 0.01,
                          "COS2");

  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x402c, 32768 * 0.001,
                          "Uab3");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x402d, 32768 * 0.001,
                          "Ubc3");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x402e, 32768 * 0.001,
                          "Uac3");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4030, 32768 * 0.12,
                          "Ia3");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4031, 32768 * 0.12,
                          "Ib3");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4032, 32768 * 0.12,
                          "Ic3");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4033, 32768 * 0.02,
                          "I03");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4034, 32768, "P3");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4036, 32768 * 0.01,
                          "COS3");

  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4040, 32768 * 0.001,
                          "Uab4");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4041, 32768 * 0.001,
                          "Ubc4");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4042, 32768 * 0.001,
                          "Uac4");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4044, 32768 * 0.12,
                          "Ia4");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4045, 32768 * 0.12,
                          "Ib4");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4046, 32768 * 0.12,
                          "Ic4");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4047, 32768 * 0.02,
                          "I04");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4048, 32768, "P4");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x404a, 32768 * 0.01,
                          "COS4");

  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4054, 32768 * 0.001,
                          "Uab5");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4055, 32768 * 0.001,
                          "Ubc5");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4056, 32768 * 0.001,
                          "Uac5");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4058, 32768 * 0.12,
                          "Ia5");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4059, 32768 * 0.12,
                          "Ib5");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x405a, 32768 * 0.12,
                          "Ic5");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x405b, 32768 * 0.02,
                          "I05");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x405c, 32768, "P5");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x405e, 32768 * 0.01,
                          "COS5");

  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4068, 32768 * 0.001,
                          "Uab6");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4069, 32768 * 0.001,
                          "Ubc6");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x406a, 32768 * 0.001,
                          "Uac6");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x406c, 32768 * 0.12,
                          "Ia6");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x406d, 32768 * 0.12,
                          "Ib6");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x406e, 32768 * 0.12,
                          "Ic6");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x406f, 32768 * 0.02,
                          "I06");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4070, 32768, "P6");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4072, 32768 * 0.01,
                          "COS6");

  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x407c, 32768 * 0.001,
                          "Uab7");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x407d, 32768 * 0.001,
                          "Ubc7");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x407e, 32768 * 0.001,
                          "Uac7");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4080, 32768 * 0.12,
                          "Ia7");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4081, 32768 * 0.12,
                          "Ib7");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4082, 32768 * 0.12,
                          "Ic7");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4083, 32768 * 0.02,
                          "I07");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4084, 32768, "P7");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4086, 32768 * 0.01,
                          "COS7");

  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4090, 32768 * 0.001,
                          "Uab8");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4091, 32768 * 0.001,
                          "Ubc8");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4092, 32768 * 0.001,
                          "Uac8");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4094, 32768 * 0.12,
                          "Ia8");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4095, 32768 * 0.12,
                          "Ib8");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4096, 32768 * 0.12,
                          "Ic8");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4097, 32768 * 0.02,
                          "I08");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4098, 32768, "P8");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x409a, 32768 * 0.01,
                          "COS8");

  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40a4, 32768 * 0.001,
                          "Uab9");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40a5, 32768 * 0.001,
                          "Ubc9");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40a6, 32768 * 0.001,
                          "Uac9");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40a8, 32768 * 0.12,
                          "Ia9");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40a9, 32768 * 0.12,
                          "Ib9");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40aa, 32768 * 0.12,
                          "Ic9");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40ab, 32768 * 0.02,
                          "I09");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40ac, 32768, "P9");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40ae, 32768 * 0.01,
                          "COS9");

  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40b8, 32768 * 0.001,
                          "Uab10");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40b9, 32768 * 0.001,
                          "Ubc10");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40ba, 32768 * 0.001,
                          "Uac10");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40bc, 32768 * 0.12,
                          "Ia10");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40bd, 32768 * 0.12,
                          "Ib10");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40be, 32768 * 0.12,
                          "Ic10");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40bf, 32768 * 0.02,
                          "I010");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40c0, 32768, "P10");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40c2, 32768 * 0.01,
                          "COS10");

  err = rs232_PRS3342_task(prs3342);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_PRS3342_task failed", __LINE__, __func__);
    return;
  }

  // debug here
  /*
  for (int i = 0; i < 31; i++) {
    prs3342->datas[i]->data = i;
  }*/

  rs232_lora_ebyte_data_t *ebyte_data = NULL;
  int interval = 60 * 1000;
  while (true) {
    if (esp_timer_get_time() / 1000000 - prs3342->datas[0]->update_time <=
            interval / 1000 &&
        prs3342->datas[0]->update_time != 0) {
      vTaskDelay(10 * 1000 / portTICK_RATE_MS);
      ebyte_data = rs232_lora_ebyte_new_data();
      ebyte_data->id = 1;
      ebyte_data->type = 2;
      ebyte_data->cmd = 3;
      ebyte_data->handle = 4;
      ebyte_data->len = prs3342->data_num * 4;
      ebyte_data->data = malloc(ebyte_data->len);
      for (int i = 0; i < prs3342->data_num; i++) {
        memcpy(ebyte_data->data + 4 * i, &prs3342->datas[i]->data, 4);
        printf("index:%d data:%f\n", i, prs3342->datas[i]->data);
      }
      rs232_lora_ebyte_sent(ebyte_data);
      rs232_lora_ebyte_free_data(ebyte_data);
    }
    vTaskDelay(interval / portTICK_RATE_MS);
  }

  ESP_LOGI(TAG, "test end");
}
