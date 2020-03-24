#ifndef _RS232_LORA_EBYTE_H
#define _RS232_LORA_EBYTE_H

#include "esp_err.h"
#include "rs232_dev.h"

#define RS232_LORA_EBYTE_DATA_HEAD_SIZE 1
#define RS232_LORA_EBYTE_ID_SIZE 2
#define RS232_LORA_EBYTE_TYPE_SIZE 2
#define RS232_LORA_EBYTE_CMD_SIZE 2
#define RS232_LORA_EBYTE_SESSION_SIZE 4
#define RS232_LORA_EBYTE_LEN_SIZE 1
#define RS232_LORA_EBYTE_CRC_SIZE 1
#define RS232_LORA_EBYTE_DATA_END_SIZE 1
#define RS232_LORA_EBYTE_MIN_DATA_SIZE                                         \
  (RS232_LORA_EBYTE_DATA_HEAD_SIZE + RS232_LORA_EBYTE_ID_SIZE +                \
   RS232_LORA_EBYTE_TYPE_SIZE + RS232_LORA_EBYTE_CMD_SIZE +                    \
   RS232_LORA_EBYTE_SESSION_SIZE + RS232_LORA_EBYTE_LEN_SIZE +                 \
   RS232_LORA_EBYTE_CRC_SIZE + RS232_LORA_EBYTE_DATA_END_SIZE)

#define RS232_LORA_EBYTE_UNARYCALL_TIMEOUT_DEFAULT (5000 / portTICK_RATE_MS)

enum rs232_lora_ebyte_cmd_type_t {
  RS232_LORA_EBYTE_CMD_TYPE_FLOW = 0,
  RS232_LORA_EBYTE_CMD_TYPE_UNARYCALL,
  RS232_LORA_EBYTE_CMD_TYPE_STREAMCALL,
};

typedef struct _rs232_lora_ebyte_data_t {
  int16_t id;
  int16_t type;
  int16_t cmd;
  QueueHandle_t handle;
  int16_t len;
  uint8_t *data;
  uint32_t timeout;
} rs232_lora_ebyte_data_t;

typedef struct _rs232_lora_ebyte_t {
  rs232_dev_config_t config;
} rs232_lora_ebyte_t;

rs232_lora_ebyte_data_t *rs232_lora_ebyte_new_data();

void rs232_lora_ebyte_free_data(rs232_lora_ebyte_data_t *ebyte_data);

esp_err_t rs232_lora_ebyte_init(int uart_num, int rx_pin, int tx_pin,
                                int baud_rate);

esp_err_t rs232_lora_ebyte_sent(rs232_lora_ebyte_data_t *ebyte_data);

rs232_lora_ebyte_data_t *rs232_lora_ebyte_recv();

// need task
rs232_lora_ebyte_data_t *
rs232_lora_ebyte_sent_and_wait_finish(rs232_lora_ebyte_data_t *ebyte_data);

esp_err_t rs232_lora_ebyte_task();

#endif
