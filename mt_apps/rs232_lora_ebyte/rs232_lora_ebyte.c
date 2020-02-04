#include "string.h"

#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "rs232_lora_ebyte.h"
#include "rs232_lora_ebyte_flow_manage.h"
#include "rs232_lora_ebyte_module_manage.h"
#include "rs232_lora_ebyte_unarycall_manage.h"

#include "mt_module_mqtt.h"

// global define ==============================================================

static const char *TAG = "RS232_LORA_EBYTE";

static rs232_dev_config_t *CONFIG = NULL;
uint8_t RS232_LORA_EBYTE_DATA_HEAD = 0x61;
uint8_t RS232_LORA_EBYTE_DATA_END = 0x16;

// static func ================================================================

static esp_err_t
rs232_lora_ebyte_parse_flow(rs232_lora_ebyte_data_t *ebyte_data) {
  esp_err_t err = ESP_OK;
  BaseType_t ret = pdFALSE;
  rs232_lora_ebyte_flow_manage_t *manage = NULL;
  int match = 0;
  int timeout = 100;
  mt_module_mqtt_msg_t *mqtt_msg = malloc(sizeof(mt_module_mqtt_msg_t));

  ESP_LOGI(TAG, "%4d %s id:%d", __LINE__, __func__, ebyte_data->id);

  manage = rs232_lora_ebyte_flow_manage_get();
  if (manage == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_flow_manage_get NULL", __LINE__,
             __func__);
    return ESP_ERR_INVALID_ARG;
  }

  for (int i = 0; i < manage->flows_size; i++) {
    if (manage->flows_addr[i] == ebyte_data->id) {
      char *topic = malloc(120);
      match++;
      sprintf(topic, "mt/devices/%s/flow_channel/sessions/%s/upstream",
              manage->flows[i]->module_http->module->deviceID,
              manage->flows[i]->session);
      mqtt_msg->topic = topic;
      mqtt_msg->buf = ebyte_data->data;
      mqtt_msg->buf_size = ebyte_data->len;

      ret = xQueueSend(manage->flows_handle[i], &mqtt_msg, timeout);
      if (ret != pdTRUE) {
        ESP_LOGE(TAG, "%4d %s xQueueSend failed", __LINE__, __func__);
        err = ESP_ERR_INVALID_ARG;
        goto EXIT;
      }
    }
  }

  if (match == 0) {
    ESP_LOGE(TAG, "%4d %s id:%d unmatch id in flows", __LINE__, __func__,
             ebyte_data->id);
  } else if (match > 1) {
    ESP_LOGW(TAG, "%4d %s id:%d match %d times", __LINE__, __func__,
             ebyte_data->id, match);
  }

EXIT:
  if (mqtt_msg != NULL) {
    // free(mqtt_msg);
  }
  return err;
}

static esp_err_t
rs232_lora_ebyte_parse_unarycall(rs232_lora_ebyte_data_t *ebyte_data) {
  esp_err_t err = ESP_OK;
  BaseType_t ret = pdFALSE;

  if (ebyte_data->handle == 0) {
    ESP_LOGE(TAG, "%4d %s ebyte_data->handle zero", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  ret = xQueueSend(ebyte_data->handle, &ebyte_data, 0);
  if (ret != pdTRUE) {
    ESP_LOGE(TAG, "%4d %s xQueueSend:%p failed", __LINE__, __func__,
             ebyte_data->handle);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return err;
}

static esp_err_t
rs232_lora_ebyte_parse_streamcall(rs232_lora_ebyte_data_t *ebyte_data) {
  esp_err_t err = ESP_OK;

  return err;
}

static esp_err_t
rs232_lora_ebyte_data_parse_dispatch(rs232_lora_ebyte_data_t *ebyte_data) {
  esp_err_t err = ESP_OK;

  switch (ebyte_data->cmd) {
  case RS232_LORA_EBYTE_CMD_TYPE_FLOW:
    err = rs232_lora_ebyte_parse_flow(ebyte_data);
    break;
  case RS232_LORA_EBYTE_CMD_TYPE_UNARYCALL:
    err = rs232_lora_ebyte_parse_unarycall(ebyte_data);
    break;
  case RS232_LORA_EBYTE_CMD_TYPE_STREAMCALL:
    err = rs232_lora_ebyte_parse_streamcall(ebyte_data);
    break;
  default:
    ESP_LOGE(TAG, "%4d %s unexcept cmd:%d", __LINE__, __func__,
             ebyte_data->cmd);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

EXIT:
  return err;
}

static rs232_lora_ebyte_data_t *rs232_lora_ebyte_data_parse(uint8_t *buf,
                                                            int buf_size) {
  esp_err_t err = ESP_OK;
  int offset = 0;
  rs232_lora_ebyte_data_t *ebyte_data = NULL;

  // new data
  ebyte_data = rs232_lora_ebyte_new_data();

  // parse buf size
  if (buf_size < RS232_LORA_EBYTE_MIN_DATA_SIZE) {
    ESP_LOGE(TAG,
             "%4d %s buf size:%d less than RS232_LORA_EBYTE_MIN_DATA_SIZE:%d",
             __LINE__, __func__, buf_size, RS232_LORA_EBYTE_MIN_DATA_SIZE);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // parse head
  if (buf[offset] != RS232_LORA_EBYTE_DATA_HEAD) {
    ESP_LOGE(TAG, "%4d %s buf head:%d not match RS232_LORA_EBYTE_DATA_HEAD:%d",
             __LINE__, __func__, buf[offset], RS232_LORA_EBYTE_DATA_HEAD);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  offset += RS232_LORA_EBYTE_DATA_HEAD_SIZE;

  // parse id
  for (int i = 0; i < RS232_LORA_EBYTE_ID_SIZE; i++) {
    ebyte_data->id += (int)(buf[offset + i])
                      << (8 * (RS232_LORA_EBYTE_ID_SIZE - i - 1));
  }
  offset += RS232_LORA_EBYTE_ID_SIZE;

  // parse type
  for (int i = 0; i < RS232_LORA_EBYTE_TYPE_SIZE; i++) {
    ebyte_data->type += (int)(buf[offset + i])
                        << (8 * (RS232_LORA_EBYTE_TYPE_SIZE - i - 1));
  }
  offset += RS232_LORA_EBYTE_TYPE_SIZE;

  // parse cmd
  for (int i = 0; i < RS232_LORA_EBYTE_CMD_SIZE; i++) {
    ebyte_data->cmd += (int)(buf[offset + i])
                       << (8 * (RS232_LORA_EBYTE_CMD_SIZE - i - 1));
  }
  offset += RS232_LORA_EBYTE_CMD_SIZE;

  // parse handle
  int handle = 0;
  for (int i = 0; i < RS232_LORA_EBYTE_SESSION_SIZE; i++) {
    handle += (int)(buf[offset + i])
              << (8 * (RS232_LORA_EBYTE_SESSION_SIZE - i - 1));
  }
  ebyte_data->handle = (QueueHandle_t)handle;
  offset += RS232_LORA_EBYTE_SESSION_SIZE;

  // parse len
  for (int i = 0; i < RS232_LORA_EBYTE_LEN_SIZE; i++) {
    ebyte_data->len += (buf[offset + i])
                       << (8 * (RS232_LORA_EBYTE_LEN_SIZE - i - 1));
  }
  offset += RS232_LORA_EBYTE_LEN_SIZE;

  // parse data
  if (ebyte_data->len != buf_size - RS232_LORA_EBYTE_MIN_DATA_SIZE) {
    ESP_LOGE(TAG, "%4d %s ebyte_data->len:%d not match except len:%d", __LINE__,
             __func__, ebyte_data->len,
             buf_size - RS232_LORA_EBYTE_MIN_DATA_SIZE);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  ebyte_data->data = malloc(ebyte_data->len);
  memcpy(ebyte_data->data, &buf[offset], ebyte_data->len);
  offset += ebyte_data->len;

  // parse crc
  uint8_t crc = 0;
  for (int i = 0;
       i < ebyte_data->len + RS232_LORA_EBYTE_MIN_DATA_SIZE -
               RS232_LORA_EBYTE_CRC_SIZE - RS232_LORA_EBYTE_DATA_END_SIZE;
       i++) {
    crc += buf[i];
  }
  if (crc != buf[offset]) {
    ESP_LOGE(TAG, "%4d %s crc error", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  offset += RS232_LORA_EBYTE_CRC_SIZE;

  // parse end
  if (buf[offset] != RS232_LORA_EBYTE_DATA_END) {
    ESP_LOGE(TAG, "%4d %s buf end:%d not match RS232_LORA_EBYTE_DATA_END:%d",
             __LINE__, __func__, buf[offset], RS232_LORA_EBYTE_DATA_END);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  offset += RS232_LORA_EBYTE_DATA_END_SIZE;

  ESP_LOGW(TAG, "%4d %s id:%d, type:%d, cmd:%d, handle:%p, len:%d", __LINE__,
           __func__, ebyte_data->id, ebyte_data->type, ebyte_data->cmd,
           ebyte_data->handle, ebyte_data->len);

EXIT:
  if (err != ESP_OK) {
    rs232_lora_ebyte_free_data(ebyte_data);
    return NULL;
  }

  return ebyte_data;
}

static uint8_t *rs232_lora_ebyte_gen_data(rs232_lora_ebyte_data_t *ebyte_data,
                                          int *buf_size_out) {
  esp_err_t err = ESP_OK;
  uint8_t *buf_out = NULL;
  int offset = 0;

  // check ebyte
  if (ebyte_data == NULL) {
    ESP_LOGE(TAG, "%4d %s ebyte data NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  *buf_size_out = ebyte_data->len + RS232_LORA_EBYTE_MIN_DATA_SIZE;
  buf_out = malloc(*buf_size_out);

  // gen head
  buf_out[offset] = RS232_LORA_EBYTE_DATA_HEAD;
  offset += RS232_LORA_EBYTE_DATA_HEAD_SIZE;

  // gen id
  for (int i = 0; i < RS232_LORA_EBYTE_ID_SIZE; i++) {
    buf_out[offset + i] =
        (uint8_t)(ebyte_data->id >> (8 * (RS232_LORA_EBYTE_ID_SIZE - i - 1)));
  }
  offset += RS232_LORA_EBYTE_ID_SIZE;

  // gen type
  for (int i = 0; i < RS232_LORA_EBYTE_TYPE_SIZE; i++) {
    buf_out[offset + i] = (uint8_t)(ebyte_data->type >>
                                    (8 * (RS232_LORA_EBYTE_TYPE_SIZE - i - 1)));
  }
  offset += RS232_LORA_EBYTE_TYPE_SIZE;

  // gen cmd
  for (int i = 0; i < RS232_LORA_EBYTE_CMD_SIZE; i++) {
    buf_out[offset + i] =
        (uint8_t)(ebyte_data->cmd >> (8 * (RS232_LORA_EBYTE_CMD_SIZE - i - 1)));
  }
  offset += RS232_LORA_EBYTE_CMD_SIZE;

  // gen handle
  int handle = (int)ebyte_data->handle;
  for (int i = 0; i < RS232_LORA_EBYTE_SESSION_SIZE; i++) {
    buf_out[offset + i] =
        (uint8_t)(handle >> (8 * (RS232_LORA_EBYTE_SESSION_SIZE - i - 1)));
  }
  offset += RS232_LORA_EBYTE_SESSION_SIZE;

  // gen len
  for (int i = 0; i < RS232_LORA_EBYTE_LEN_SIZE; i++) {
    buf_out[offset + i] =
        (uint8_t)(ebyte_data->len >> (8 * (RS232_LORA_EBYTE_LEN_SIZE - i - 1)));
  }
  offset += RS232_LORA_EBYTE_LEN_SIZE;

  // gen data
  memcpy(&buf_out[offset], ebyte_data->data, ebyte_data->len);
  offset += ebyte_data->len;

  // gen crc
  uint8_t crc = 0;
  for (int i = 0;
       i < ebyte_data->len + RS232_LORA_EBYTE_MIN_DATA_SIZE -
               RS232_LORA_EBYTE_CRC_SIZE - RS232_LORA_EBYTE_DATA_END_SIZE;
       i++) {
    crc += buf_out[i];
  }
  buf_out[offset] = crc;
  offset += RS232_LORA_EBYTE_CRC_SIZE;

  // gen end
  buf_out[offset] = RS232_LORA_EBYTE_DATA_END;

EXIT:
  if (err != ESP_OK) {
    *buf_size_out = 0;
    free(buf_out);
  }

  return buf_out;
}

static void rs232_lora_ebyte_loop() {
  while (1) {
    int read_size = 0;
    esp_err_t err = ESP_OK;
    uint8_t *read_buf = NULL;
    rs232_lora_ebyte_data_t *ebyte_data = NULL;

    // read_buf = rs232_dev_read_debug(CONFIG, &read_size);
    read_buf = rs232_dev_read(CONFIG, &read_size);
    // read_size = 0; // debug

    if (read_size > 0) {
      ESP_LOGI(TAG, "%4d %s rs232_dev_read, size=%d", __LINE__, __func__,
               read_size);
      for (int i = 0; i < read_size; i++) {
        printf("%2x ", read_buf[i]);
      }
      printf("\n");
      ebyte_data = rs232_lora_ebyte_data_parse(read_buf, read_size);
      if (ebyte_data == NULL) {
        ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_data_parse get NULL", __LINE__,
                 __func__);
        goto CLEAN;
      }
      // msg dispatch
      err = rs232_lora_ebyte_data_parse_dispatch(ebyte_data);
      if (err != ESP_OK) {
        ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_data_parse_dispatch failed",
                 __LINE__, __func__);
      }

    CLEAN:
      if (read_buf != NULL) {
        free(read_buf);
      }
    }
    vTaskDelay(10 / portTICK_RATE_MS);
  }
  vTaskDelete(NULL);
}

// global func
// ================================================================

rs232_lora_ebyte_data_t *rs232_lora_ebyte_new_data() {
  rs232_lora_ebyte_data_t *ebyte_data = malloc(sizeof(rs232_lora_ebyte_data_t));
  ebyte_data->id = 0;
  ebyte_data->type = 0;
  ebyte_data->cmd = 0;
  ebyte_data->len = 0;
  ebyte_data->handle = 0;
  ebyte_data->data = NULL;
  ebyte_data->timeout = RS232_LORA_EBYTE_UNARYCALL_TIMEOUT_DEFAULT;

  return ebyte_data;
}

void rs232_lora_ebyte_free_data(rs232_lora_ebyte_data_t *ebyte_data) {
  if (ebyte_data == NULL) {
    return;
  }

  if (ebyte_data->data != NULL) {
    free(ebyte_data->data);
  }

  free(ebyte_data);
}

esp_err_t rs232_lora_ebyte_init(int uart_num, int rx_pin, int tx_pin,
                                int baud_rate) {
  esp_err_t err = ESP_OK;
  if (CONFIG != NULL) {
    rs232_dev_config_free(CONFIG);
  }

  CONFIG = rs232_dev_default_new();

  CONFIG->uart_num = uart_num;
  CONFIG->rx_pin = rx_pin;
  CONFIG->tx_pin = tx_pin;
  CONFIG->uart_config->baud_rate = baud_rate;

  err = rs232_dev_init(CONFIG);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_dev_init failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  if (err != ESP_OK) {
    rs232_dev_config_free(CONFIG);
  }

  return ESP_OK;
}

esp_err_t rs232_lora_ebyte_sent(rs232_lora_ebyte_data_t *ebyte_data) {
  esp_err_t err = ESP_OK;
  uint8_t *buf = NULL;
  int buf_size = 0;

  buf = rs232_lora_ebyte_gen_data(ebyte_data, &buf_size);
  if (buf == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_gen_data failed", __LINE__,
             __func__);
    goto EXIT;
  }

  ESP_LOGW(TAG, "%4d %s sent size:%d", __LINE__, __func__, buf_size);

  rs232_dev_write(CONFIG, buf, buf_size);

EXIT:
  if (buf != NULL)
    free(buf);
  return err;
}

rs232_lora_ebyte_data_t *rs232_lora_ebyte_recv() {
  esp_err_t err = ESP_OK;
  rs232_lora_ebyte_data_t *ebyte_recv = NULL;
  int read_size = 0;
  uint8_t *read_buf = NULL;

  read_buf = rs232_dev_read(CONFIG, &read_size);

  if (read_size > 0) {
    ESP_LOGI(TAG, "%4d %s rs232_dev_read, size=%d", __LINE__, __func__,
             read_size);
    for (int i = 0; i < read_size; i++) {
      printf("%2x ", read_buf[i]);
    }
    printf("\n");
  } else {
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  ebyte_recv = rs232_lora_ebyte_data_parse(read_buf, read_size);
  if (ebyte_recv == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_data_parse NULL", __LINE__,
             __func__);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

EXIT:
  if (read_buf != NULL) {
    free(read_buf);
  }
  if (err != ESP_OK) {
    if (ebyte_recv != NULL) {
      rs232_lora_ebyte_free_data(ebyte_recv);
    }
    return NULL;
  }
  return ebyte_recv;
}

// need task, this func will block
rs232_lora_ebyte_data_t *
rs232_lora_ebyte_sent_and_wait_finish(rs232_lora_ebyte_data_t *ebyte_data) {
  esp_err_t err = ESP_OK;
  rs232_lora_ebyte_data_t *ebyte_data_out = rs232_lora_ebyte_new_data();

  ebyte_data->handle = xQueueCreate(1, sizeof(rs232_lora_ebyte_data_t));

  err = rs232_lora_ebyte_unarycall_manage_add(ebyte_data->handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_unarycall_manage_add failed",
             __LINE__, __func__);
    goto EXIT;
  }

  err = rs232_lora_ebyte_sent(ebyte_data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_sent failed", __LINE__, __func__);
    goto EXIT;
  }

  if (xQueueReceive(ebyte_data->handle, ebyte_data_out, ebyte_data->timeout) !=
      pdTRUE) {
    ESP_LOGE(TAG, "%4d %s xQueueReceive failed", __LINE__, __func__);
    err = ESP_ERR_TIMEOUT;
    goto EXIT;
  }

  ESP_LOGI(TAG, "%4d %s lora_ebyte request success", __LINE__, __func__);

EXIT:
  if (err != ESP_OK) {
    if (err == ESP_ERR_TIMEOUT) {
      ESP_LOGW(TAG, "%4d %s lora ebyte request timeout", __LINE__, __func__);
    } else {
      ESP_LOGE(TAG, "%4d %s lora ebyte request error", __LINE__, __func__);
    }
    rs232_lora_ebyte_free_data(ebyte_data_out);
    ebyte_data_out = NULL;
  }
  err = rs232_lora_ebyte_unarycall_manage_del(ebyte_data->handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_unarycall_manage_del failed",
             __LINE__, __func__);
  }
  return ebyte_data_out;
}

esp_err_t rs232_lora_ebyte_task() {
  esp_err_t err = ESP_OK;

  ESP_LOGI(TAG, "%4d %s task begin", __LINE__, __func__);

  err = rs232_lora_ebyte_module_manage_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_module_manage_init failed", __LINE__,
             __func__);
    return err;
  }

  xTaskCreate(rs232_lora_ebyte_loop, "EBYTE_TASK", 8 * 1024, NULL, 10, NULL);

  return ESP_OK;
}
