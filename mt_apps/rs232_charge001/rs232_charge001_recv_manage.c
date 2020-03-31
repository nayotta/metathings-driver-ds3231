#include "rs232_charge001_recv_manage.h"

// static define ==============================================================

static const char *TAG = "RS232_CHARGE001_RECV_MANAGE";

#define RS232_CHARGE001_MAX_BUF_SIZE (1024)
static uint8_t RECV_BUF[RS232_CHARGE001_MAX_BUF_SIZE];
static int RECV_SIZE = 0;

// static func ================================================================

static void rs232_charge001_recv_manage_parse_msg() {
  if (RECV_SIZE <= 2) {
    goto EXIT;
  }

  ESP_LOGI(TAG, "%4d %s recv msg:%2d---> %s", __LINE__, __func__,
           strlen((char *)RECV_BUF), RECV_BUF);
  // TODO(ZH) parse msg
EXIT:
  RECV_BUF[0] = '\0';
  RECV_SIZE = 0;
}

static void rs232_charge001_recv_manage_push_byte(uint8_t data) {
  if (RECV_SIZE < RS232_CHARGE001_MAX_BUF_SIZE) {
    RECV_BUF[RECV_SIZE] = data;
    RECV_BUF[RECV_SIZE + 1] = '\0';
    RECV_SIZE++;
  } else {
    ESP_LOGE(TAG, "%4d %s RECV_SIZE reach max:%d", __LINE__, __func__,
             RECV_SIZE);
    return;
  }

  // one cmd
  if (data == '\n') {
    RECV_BUF[RECV_SIZE - 1] = '\0';
    rs232_charge001_recv_manage_parse_msg();
  }
}

static void rs232_charge001_recv_manage_loop(rs232_dev_config_t *dev_config) {
  int len = 0;
  uint8_t data = 0;

  while (true) {
    len = uart_read_bytes(dev_config->uart_num, &data, 1,
                          20 / portTICK_PERIOD_MS);
    {
      if (len == 1) {
        // debug here
        printf("%2x ", data);
        rs232_charge001_recv_manage_push_byte(data);
      }
    }
  }
}

// global func ================================================================

esp_err_t rs232_charge001_recv_manage_init(rs232_dev_config_t *dev_config) {
  esp_err_t err = ESP_OK;

  xTaskCreate((TaskFunction_t)rs232_charge001_recv_manage_loop,
              "CHARGE001_RECV_TASK", 8 * 1024, dev_config, 8, NULL);

  return err;
}
