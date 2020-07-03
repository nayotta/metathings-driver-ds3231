#include "rs232_charge001_recv_manage.h"
#include "rs232_charge001.h"
#include "rs232_charge001_recv_manage_get_state.h"
#include "rs232_charge001_recv_manage_get_states.h"
#include "rs232_charge001_recv_manage_notify_coin.h"
#include "rs232_charge001_recv_manage_notify_over.h"
#include "rs232_charge001_recv_manage_set_charge.h"
#include "rs232_charge001_utils.h"

// static define ==============================================================

static const char *TAG = "RS232_CHARGE001_RECV_MANAGE";

#define RS232_CHARGE001_MAX_BUF_SIZE (64)

// static func ================================================================

static void
rs232_charge001_recv_manage_msg_dispatch(rs232_charge001_msg_t *msg) {
  switch (msg->cmd) {
  case RS232_CHARGE001_CMD_TYPE_SET_CHARGE:
    rs232_charge001_recv_manage_set_charge_parse(msg->buf, msg->size);
    break;
  case RS232_CHARGE001_CMD_TYPE_GET_STATE:
    rs232_charge001_recv_manage_get_state_parse(msg->buf, msg->size);
    break;
  case RS232_CHARGE001_CMD_TYPE_GET_STATES:
    rs232_charge001_recv_manage_get_states_parse(msg->buf, msg->size);
    break;
  case RS232_CHARGE001_CMD_TYPE_NOTIFY_OVER:
    rs232_charge001_recv_manage_notify_over_push_flow(msg->buf, msg->size);
    break;
  case RS232_CHARGE001_CMD_TYPE_NOTIFY_COIN:
    rs232_charge001_recv_manage_notify_coin_push_flow(msg->buf, msg->size);
    break;
  default:
    ESP_LOGE(TAG, "%4d %s unexcept cmd type:%d", __LINE__, __func__, msg->cmd);
    return;
  }
}

static void
rs232_charge001_recv_manage_parse_msg_task(rs232_charge001_recv_buf_t *data) {
  rs232_charge001_msg_t *msg = NULL;

  if (data->size <= 2) {
    goto EXIT;
  }

  ESP_LOGI(TAG, "%4d %s recv msg:%2d--->", __LINE__, __func__, data->size);

  msg = rs232_chrage001_utils_unmarshal_buf(data->buf, data->size);
  if (msg == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_chrage001_utils_unmarshal_buf NULL", __LINE__,
             __func__);
    goto EXIT;
  }

  rs232_charge001_recv_manage_msg_dispatch(msg);

EXIT:
  rs232_charge001_free_recv_buf(data);
  rs232_charge001_utils_free_msg(msg);
  vTaskDelete(NULL);
}

static void rs232_charge001_recv_manage_loop(rs232_dev_config_t *dev_config) {
  int len = 0;
  uint8_t data[RS232_CHARGE001_MAX_BUF_SIZE] = "";

  while (true) {
    len = uart_read_bytes(dev_config->uart_num, data, 50,
                          dev_config->timeout / portTICK_PERIOD_MS);
    {
      if (len >= 1) {
        // debug here
        printf("recv:");
        for (int i = 0; i < len; i++)
          printf("%2x ", data[i]);
        printf("\n");
        rs232_charge001_recv_buf_t *recv_buf =
            rs232_charge001_new_recv_buf(data, len);
        xTaskCreate((TaskFunction_t)rs232_charge001_recv_manage_parse_msg_task,
                    "parse_task", 8 * 1024, recv_buf, 12, NULL);
      }
    }
  }
}

// help func ==================================================================

rs232_charge001_recv_buf_t *rs232_charge001_new_recv_buf(uint8_t *buf,
                                                         uint32_t size) {
  rs232_charge001_recv_buf_t *data = malloc(sizeof(rs232_charge001_recv_buf_t));

  data->buf = malloc(size);
  memcpy(data->buf, buf, size);
  data->size = size;

  return data;
}

void rs232_charge001_free_recv_buf(rs232_charge001_recv_buf_t *data) {
  if (data == NULL)
    return;

  if (data->buf != NULL)
    free(data->buf);

  free(data);
}

// global func ================================================================

esp_err_t rs232_charge001_recv_manage_init(rs232_dev_config_t *dev_config) {
  esp_err_t err = ESP_OK;

  xTaskCreate((TaskFunction_t)rs232_charge001_recv_manage_loop,
              "CHARGE001_RECV_TASK", 8 * 1024, dev_config, 8, NULL);

  ESP_LOGI(TAG, "%4d %s init success", __LINE__, __func__);

  return err;
}
