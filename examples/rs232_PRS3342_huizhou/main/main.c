#include "esp_log.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

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
  prs3342->data_num = 90;
  prs3342->datas = malloc(prs3342->data_num * sizeof(rs232_PRS3342_data_t *));
  for (int i = 0; i < prs3342->data_num; i++) {
    prs3342->datas[i] = malloc(sizeof(rs232_PRS3342_data_t));
  }

  int offset = 0;
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4004, 1, "Uab1");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4005, 1, "Ubc1");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4006, 1, "Uac1");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4008, 1, "Ia1");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4009, 1, "Ib1");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x400a, 1, "Ic1");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x400b, 1, "I01");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x400c, 1, "P1");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x400e, 1, "COS1");

  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4018, 1, "Uab2");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4019, 1, "Ubc2");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x401a, 1, "Uac2");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x401c, 1, "Ia2");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x401d, 1, "Ib2");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x401e, 1, "Ic2");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x401f, 1, "I02");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4020, 1, "P2");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4022, 1, "COS2");

  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x402c, 1, "Uab3");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x402d, 1, "Ubc3");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x402e, 1, "Uac3");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4030, 1, "Ia3");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4031, 1, "Ib3");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4032, 1, "Ic3");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4033, 1, "I03");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4034, 1, "P3");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4036, 1, "COS3");

  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4040, 1, "Uab4");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4041, 1, "Ubc4");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4042, 1, "Uac4");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4044, 1, "Ia4");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4045, 1, "Ib4");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4046, 1, "Ic4");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4047, 1, "I04");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4048, 1, "P4");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x404a, 1, "COS4");

  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4054, 1, "Uab5");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4055, 1, "Ubc5");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4056, 1, "Uac5");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4058, 1, "Ia5");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4059, 1, "Ib5");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x405a, 1, "Ic5");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x405b, 1, "I05");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x405c, 1, "P5");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x405e, 1, "COS5");

  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4068, 1, "Uab6");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4069, 1, "Ubc6");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x406a, 1, "Uac6");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x406c, 1, "Ia6");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x406d, 1, "Ib6");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x406e, 1, "Ic6");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x406f, 1, "I06");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4070, 1, "P6");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4072, 1, "COS6");

  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x407c, 1, "Uab7");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x407d, 1, "Ubc7");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x407e, 1, "Uac7");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4080, 1, "Ia7");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4081, 1, "Ib7");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4082, 1, "Ic7");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4083, 1, "I07");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4084, 1, "P7");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4086, 1, "COS7");

  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4090, 1, "Uab8");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4091, 1, "Ubc8");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4092, 1, "Uac8");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4094, 1, "Ia8");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4095, 1, "Ib8");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4096, 1, "Ic8");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4097, 1, "I08");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x4098, 1, "P8");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x409a, 1, "COS8");

  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40a4, 1, "Uab9");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40a5, 1, "Ubc9");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40a6, 1, "Uac9");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40a8, 1, "Ia9");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40a9, 1, "Ib9");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40aa, 1, "Ic9");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40ab, 1, "I09");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40ac, 1, "P9");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40ae, 1, "COS9");

  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40b8, 1, "Uab10");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40b9, 1, "Ubc10");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40ba, 1, "Uac10");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40bc, 1, "Ia10");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40bd, 1, "Ib10");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40be, 1, "Ic10");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40bf, 1, "I010");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40c0, 1, "P10");
  rs232_PRS3342_new_guiyi(prs3342->datas[offset++], 0x40c2, 1, "COS10");

  err = rs232_PRS3342_task(prs3342);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_PRS3342_task failed", __LINE__, __func__);
    return;
  }

  ESP_LOGI(TAG, "test end");
}
