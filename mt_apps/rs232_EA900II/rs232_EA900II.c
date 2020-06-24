#include "math.h"
#include "string.h"

#include "esp_err.h"
#include "esp_log.h"

#include "rs232_EA900II.h"

// global define ==============================================================

static const char *TAG = "RS232_EA900II";

static int FRESH_INTERVAL = 5000; // ms

static rs232_EA900II_status_t *EA_STATUS = NULL;
static rs232_EA900II_model_t *MODEL = NULL;
static rs232_EA900II_config_t *CONFIG = NULL;

// static func ================================================================

static esp_err_t rs232_EA900II_parse_bool_string(uint8_t *buf, int offset,
                                                 bool *out) {
  *out = false;
  if (buf[offset] == '0') {
    *out = false;
  } else if (buf[offset] == '1') {
    *out = true;
  } else {
    ESP_LOGE(TAG, "%4d %s unxecpet string:%c", __LINE__, __func__, buf[offset]);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}

static esp_err_t rs232_EA900II_parse_float_string(uint8_t *buf, int offset,
                                                  int int_size, int point_size,
                                                  double *float_out) {
  *float_out = 0;

  for (int i = 0; i < int_size; i++) {
    if (buf[offset + i] < '0' || buf[offset + i] > '9') {
      ESP_LOGE(TAG, "%4d %s unexcept string:%c", __LINE__, __func__,
               buf[offset + i]);
      return ESP_ERR_INVALID_ARG;
    }
    char temp[2];
    memcpy(temp, &buf[offset + i], 1);
    temp[1] = '\0';
    *float_out += (double)atoi(temp) * pow(10, int_size - i - 1);
  }

  if (point_size > 0) {
    if (buf[offset + int_size] != '.') {
      ESP_LOGE(TAG, "%4d %s point not match", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }

    for (int i = 0; i < point_size; i++) {
      if (buf[offset + int_size + 1 + i] < '0' ||
          buf[offset + int_size + 1 + i] > '9') {
        ESP_LOGE(TAG, "%4d %s unexcept string:%c", __LINE__, __func__,
                 buf[offset + int_size + 1 + i]);
        return ESP_ERR_INVALID_ARG;
      }
      char temp[1];
      memcpy(temp, &buf[offset + int_size + 1 + i], 1);
      temp[1] = '\0';
      *float_out += (double)atoi(temp) * pow(10, -(i + 1));
    }
  }

  return ESP_OK;
}

static esp_err_t rs232_EA900II_sent_status_req(rs232_dev_config_t *config) {
  esp_err_t err = ESP_OK;
  uint8_t buf[64] = "Q1\n";

  err = rs232_dev_write(config, buf, strlen((char *)buf));
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_dev_write failed", __LINE__, __func__);
    return err;
  }

  return ESP_OK;
}

static esp_err_t rs232_EA900II_sent_model_req(rs232_dev_config_t *config) {
  esp_err_t err = ESP_OK;
  uint8_t buf[64] = "I\n";

  err = rs232_dev_write(config, buf, strlen((char *)buf));
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_dev_write failed", __LINE__, __func__);
    return err;
  }

  return ESP_OK;
}

static esp_err_t rs232_EA900II_sent_config_req(rs232_dev_config_t *config) {
  esp_err_t err = ESP_OK;
  uint8_t buf[64] = "F\n";

  err = rs232_dev_write(config, buf, strlen((char *)buf));
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_dev_write failed", __LINE__, __func__);
    return err;
  }

  return ESP_OK;
}

static rs232_EA900II_status_t *rs232_EA900II_parse_status_res(uint8_t *buf,
                                                              int buf_size) {
  esp_err_t err = ESP_OK;
  rs232_EA900II_status_t *status = malloc(sizeof(rs232_EA900II_status_t));

  if (buf_size != 47) {
    ESP_LOGE(TAG, "%4d %s get unexcept buf_size:%d", __LINE__, __func__,
             buf_size);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // bit 0
  if (buf[0] != '(') {
    ESP_LOGE(TAG, "%4d %s bit 0 want:%x but get:%x", __LINE__, __func__, '(',
             buf[0]);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // bit 1-5
  err = rs232_EA900II_parse_float_string(buf, 1, 3, 1, &status->vIn);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s parse tatus->vIn failed", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 6
  if (buf[6] != ' ') {
    ESP_LOGE(TAG, "%4d %s bit 6 not match", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 7-11
  if (buf[7] != '0' || buf[8] != '0' || buf[9] != '0' || buf[10] != '.' ||
      buf[11] != '0') {
    ESP_LOGE(TAG, "%4d %s unuse 000.0 not match", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 12
  if (buf[12] != ' ') {
    ESP_LOGE(TAG, "%4d %s bit 12 not match", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 13-17
  err = rs232_EA900II_parse_float_string(buf, 13, 3, 1, &status->vOut);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s parse status->vOut failed", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 18
  if (buf[18] != ' ') {
    ESP_LOGE(TAG, "%4d %s bit 18 not match", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 19-21
  err = rs232_EA900II_parse_float_string(buf, 19, 3, 0, &status->load);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s parse status->load failed", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 22
  if (buf[22] != ' ') {
    ESP_LOGE(TAG, "%4d %s bit 22 not match", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 23-26
  err = rs232_EA900II_parse_float_string(buf, 23, 2, 1, &status->freqIn);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s parse status->freqIn failed", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 27
  if (buf[27] != ' ') {
    ESP_LOGE(TAG, "%4d %s bit 27 not match", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 28-31
  if (buf[29] == '.')
    err = rs232_EA900II_parse_float_string(buf, 28, 1, 2, &status->vBat);
  else if (buf[30] == '.')
    err = rs232_EA900II_parse_float_string(buf, 28, 2, 1, &status->vBat);
  else {
    ESP_LOGE(TAG, "%4d %s unexcept point", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s parse status->vBat failed", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 32
  if (buf[32] != ' ') {
    ESP_LOGE(TAG, "%4d %s bit 32 not match", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 33-36
  err = rs232_EA900II_parse_float_string(buf, 33, 2, 1, &status->temp);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s parse status->temp failed", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 37
  if (buf[37] != ' ') {
    ESP_LOGE(TAG, "%4d %s bit 37 not match", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 38-45
  rs232_EA900II_parse_bool_string(buf, 38, &status->w7);
  rs232_EA900II_parse_bool_string(buf, 39, &status->w6);
  rs232_EA900II_parse_bool_string(buf, 40, &status->w5);
  rs232_EA900II_parse_bool_string(buf, 41, &status->w4);
  rs232_EA900II_parse_bool_string(buf, 42, &status->w3);
  rs232_EA900II_parse_bool_string(buf, 43, &status->w2);
  rs232_EA900II_parse_bool_string(buf, 44, &status->w1);
  rs232_EA900II_parse_bool_string(buf, 45, &status->w0);

  // bit 46
  if (buf[46] != '\r') {
    ESP_LOGE(TAG, "%4d %s bit 46 not match", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  ESP_LOGW(TAG,
           "%4d %s status get res vIn=%3.2f vOut=%3.2f load=%3.2f freqIn=%3.2f "
           "vBat=%3.2f temp=%3.2f w0=%d w1=%d w2=%d w3=%d w4=%d w5=%d "
           "w6=%d w7=%d",
           __LINE__, __func__, status->vIn, status->vOut, status->load,
           status->freqIn, status->vBat, status->temp, status->w0, status->w1,
           status->w2, status->w3, status->w4, status->w5, status->w6,
           status->w7);

EXIT:
  if (err != ESP_OK) {
    free(status);
    return NULL;
  }
  return status;
}

static rs232_EA900II_model_t *rs232_EA900II_parse_model_res(uint8_t *buf,
                                                            int buf_size) {
  esp_err_t err = ESP_OK;
  rs232_EA900II_model_t *model = malloc(sizeof(rs232_EA900II_model_t));

  if (buf_size != 39) {
    ESP_LOGE(TAG, "%4d %s get unexcept buf_size:%d", __LINE__, __func__,
             buf_size);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // bit 0
  if (buf[0] != '#') {
    ESP_LOGE(TAG, "%4d %s bit 0 want:%x but get:%x", __LINE__, __func__, '#',
             buf[0]);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // bit 1-15
  memcpy(model->name, &buf[1], 15);
  model->name[15] = '\0';

  // bit 16
  if (buf[16] != ' ') {
    ESP_LOGE(TAG, "%4d %s bit 16 not match", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 17-26
  memcpy(model->model, &buf[17], 10);
  model->model[10] = '\0';

  // bit 27
  if (buf[27] != ' ') {
    ESP_LOGE(TAG, "%4d %s bit 27 not match", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 28-37
  memcpy(model->version, &buf[28], 10);
  model->version[10] = '\0';

  // bit 38
  if (buf[38] != '\r') {
    ESP_LOGE(TAG, "%4d %s bit 38 not match", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  ESP_LOGW(TAG, "%4d %s get model res name=%15s model=%10s version=%10s",
           __LINE__, __func__, model->name, model->model, model->version);

EXIT:
  if (err != ESP_OK) {
    free(model);
    return NULL;
  }
  return model;
}

static rs232_EA900II_config_t *rs232_EA900II_parse_config_res(uint8_t *buf,
                                                              int buf_size) {
  esp_err_t err = ESP_OK;
  rs232_EA900II_config_t *config = malloc(sizeof(rs232_EA900II_config_t));

  if (buf_size != 22) {
    ESP_LOGE(TAG, "%4d %s get unexcept buf_size:%d", __LINE__, __func__,
             buf_size);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // bit 0
  if (buf[0] != '#') {
    ESP_LOGE(TAG, "%4d %s bit 0 want:%x but get:%x", __LINE__, __func__, '#',
             buf[0]);
    err = ESP_ERR_INVALID_RESPONSE;
    goto EXIT;
  }

  // bit 1-5
  err = rs232_EA900II_parse_float_string(buf, 1, 3, 1, &config->votage);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s parse config->votage failed", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 6
  if (buf[6] != ' ') {
    ESP_LOGE(TAG, "%4d %s bit 6 not match", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 7-9
  err = rs232_EA900II_parse_float_string(buf, 1, 3, 0, &config->current);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s parse config->current failed", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 10
  if (buf[10] != ' ') {
    ESP_LOGE(TAG, "%4d %s bit 10 not match", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 11-15
  err = rs232_EA900II_parse_float_string(buf, 11, 3, 1, &config->batVotage);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s parse config->batVotage failed", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 16
  if (buf[16] != ' ') {
    ESP_LOGE(TAG, "%4d %s bit 16 not match", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 17-20
  err = rs232_EA900II_parse_float_string(buf, 17, 2, 1, &config->freq);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s parse config->freq failed", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // bit 21
  if (buf[21] != '\r') {
    ESP_LOGE(TAG, "%4d %s bit 21 not match", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  ESP_LOGW(TAG,
           "%4d %s get version res votage=%3.2f current=%3.2f batvotage=%3.2f "
           "freq=%3.2f",
           __LINE__, __func__, config->votage, config->current,
           config->batVotage, config->freq);

EXIT:
  if (err != ESP_OK) {
    free(config);
    return NULL;
  }
  return config;
}

static rs232_EA900II_status_t *
rs232_EA900II_req_status(rs232_dev_config_t *config) {
  int read_size = 0;
  uint8_t *read_buf = NULL;
  esp_err_t err = ESP_OK;
  rs232_EA900II_status_t *status = NULL;

  err = rs232_EA900II_sent_status_req(config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_EA900II_sent_status_req failed", __LINE__,
             __func__);
    return NULL;
  }

  read_buf = rs232_dev_read(config, &read_size);
  for (int i = 0; i < read_size; i++)
    printf("%2x ", read_buf[i]);
  printf("\n");
  if (read_buf == NULL) {
    ESP_LOGE(TAG, "%4d %s read NULL", __LINE__, __func__);
    goto EXIT;
  }

  status = rs232_EA900II_parse_status_res(read_buf, read_size);
  if (status == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_EA900II_parse_status_res NULL", __LINE__,
             __func__);
    goto EXIT;
  }

EXIT:
  if (read_buf != NULL)
    free(read_buf);
  return status;
}

static rs232_EA900II_model_t *
rs232_EA900II_req_model(rs232_dev_config_t *config) {
  int read_size = 0;
  uint8_t *read_buf = NULL;
  esp_err_t err = ESP_OK;
  rs232_EA900II_model_t *model = NULL;

  err = rs232_EA900II_sent_model_req(config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_EA900II_sent_model_req failed", __LINE__,
             __func__);
    return NULL;
  }

  read_buf = rs232_dev_read(config, &read_size);
  for (int i = 0; i < read_size; i++)
    printf("%2x ", read_buf[i]);
  printf("\n");
  if (read_buf == NULL) {
    ESP_LOGE(TAG, "%4d %s read NULL", __LINE__, __func__);
    goto EXIT;
  }

  printf("debug model res,size=%d\n", read_size);
  model = rs232_EA900II_parse_model_res(read_buf, read_size);
  if (model == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_EA900II_parse_model_res NULL", __LINE__,
             __func__);
    goto EXIT;
  }

EXIT:
  if (read_buf != NULL)
    free(read_buf);
  return model;
}

static rs232_EA900II_config_t *
rs232_EA900II_req_config(rs232_dev_config_t *config) {
  int read_size = 0;
  uint8_t *read_buf = NULL;
  esp_err_t err = ESP_OK;
  rs232_EA900II_config_t *ea_config = NULL;

  err = rs232_EA900II_sent_config_req(config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_EA900II_sent_config_req failed", __LINE__,
             __func__);
    return NULL;
  }

  read_buf = rs232_dev_read(config, &read_size);
  for (int i = 0; i < read_size; i++)
    printf("%2x ", read_buf[i]);
  printf("\n");
  if (read_buf == NULL) {
    ESP_LOGE(TAG, "%4d %s read NULL", __LINE__, __func__);
    goto EXIT;
  }

  ea_config = rs232_EA900II_parse_config_res(read_buf, read_size);
  if (ea_config == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_EA900II_parse_config_res NULL", __LINE__,
             __func__);
    goto EXIT;
  }

EXIT:
  if (read_buf != NULL)
    free(read_buf);
  return ea_config;
}

static void rs232_EA900II_loop(rs232_dev_config_t *config) {
  while (1) {
    if (MODEL == NULL) {
      MODEL = rs232_EA900II_req_model(config);
    }

    if (CONFIG == NULL) {
      CONFIG = rs232_EA900II_req_config(config);
    }

    if (EA_STATUS != NULL)
      free(EA_STATUS);
    EA_STATUS = rs232_EA900II_req_status(config);

    vTaskDelay(FRESH_INTERVAL / portTICK_RATE_MS);
  }
}

// global func ================================================================

rs232_EA900II_status_t *rs232_EA900II_get_status() {
  if (EA_STATUS != NULL) {
    rs232_EA900II_status_t *status = malloc(sizeof(rs232_EA900II_status_t));
    memcpy(status, (void *)EA_STATUS, sizeof(rs232_EA900II_status_t));
    return status;
  } else {
    return NULL;
  }
}

rs232_EA900II_model_t *rs232_EA900II_get_model() {
  if (MODEL != NULL) {
    rs232_EA900II_model_t *model = malloc(sizeof(rs232_EA900II_model_t));
    memcpy(model, (void *)MODEL, sizeof(rs232_EA900II_model_t));
    return model;
  } else {
    return NULL;
  }
}

rs232_EA900II_config_t *rs232_EA900II_get_config() {
  if (CONFIG != NULL) {
    rs232_EA900II_config_t *config = malloc(sizeof(rs232_EA900II_config_t));
    memcpy(config, (void *)CONFIG, sizeof(rs232_EA900II_config_t));
    return config;
  } else {
    return NULL;
  }
}

esp_err_t rs232_EA900II_task(int tx_pin, int rx_pin) {
  esp_err_t err = ESP_OK;
  rs232_dev_config_t *config = NULL;

  config = rs232_dev_default_new();
  config->uart_config->baud_rate = 2400;
  config->tx_pin = tx_pin;
  config->rx_pin = rx_pin;

  err = rs232_dev_init(config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_dev_init failed", __LINE__, __func__);
    goto EXIT;
  }

  xTaskCreate((TaskFunction_t)rs232_EA900II_loop, "EA900II_TASK", 8 * 1024,
              config, 12, NULL);
EXIT:
  if (err != ESP_OK)
    rs232_dev_config_free(config);

  return err;
}
