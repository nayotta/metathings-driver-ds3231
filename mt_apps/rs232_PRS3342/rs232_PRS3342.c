#include "esp_log.h"
#include "stdio.h"
#include "string.h"

#include "rs232_PRS3342.h"
#include "rs232_dev.h"

// global define ==============================================================

static const char *TAG = "rs232-PRS3342";

rs232_PRS3342_t *PRS3342 = NULL;

// static func ================================================================

static esp_err_t rs232_PRS3342_parse_yaoxin_grp_single(rs232_PRS3342_t *prs3342,
                                                       uint8_t *buf, int size,
                                                       int grp_addr,
                                                       int grp_size) {
  if (size != grp_size) {
    ESP_LOGE(TAG, "%4d %s want size %d but get %d", __LINE__, __func__,
             3 * grp_size, size);
    return ESP_ERR_INVALID_ARG;
  }

  for (int grp_index = 0; grp_index < grp_size; grp_index++) {
    int match = 0;
    bool grp_data = false;

    grp_data = (bool)(buf[grp_index] & 0x01);

    for (int i = 0; i < prs3342->data_num; i++) {
      if (prs3342->datas[i]->addr == grp_addr + grp_index) {
        prs3342->datas[i]->state = grp_data;
        prs3342->datas[i]->update_time = esp_timer_get_time() / 1000000;
        match++;
        /*ESP_LOGI(TAG, "%4d %s get addr:%4x data:%2d name:%s", __LINE__,
                   __func__, prs3342->datas[i]->addr, prs3342->datas[i]->state,
                   prs3342->datas[i]->name);*/
      }
    }

    /*
        if (match == 0) {
          ESP_LOGW(TAG, "%4d %s data no match", __LINE__, __func__);
        } else if (match > 1) {
          ESP_LOGW(TAG, "%4d %s match more:%d", __LINE__, __func__, match);
        }*/
  }

  return ESP_OK;
}

static esp_err_t rs232_PRS3342_parse_yaoxin(rs232_PRS3342_t *prs3342,
                                            uint8_t *buf, int size) {
  esp_err_t err = ESP_OK;
  int grp_size = 0;
  int grp_addr = (int)buf[13] | (int)buf[14] << 8;

  grp_size = buf[8] & 0x7F;
  switch (buf[7]) {
  case RS232_PRS3342_DATA_TYPE_BOOL:
    err = rs232_PRS3342_parse_yaoxin_grp_single(
        prs3342, &buf[15], size - 15 - 2, grp_addr, grp_size);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s rs232_PRS3342_parse_yaoxin_grp_single error",
               __LINE__, __func__);
      return ESP_ERR_INVALID_RESPONSE;
    }
    break;
  default:
    ESP_LOGE(TAG, "%4d %s unexcept type:%2x", __LINE__, __func__, buf[7]);
    return ESP_ERR_INVALID_RESPONSE;
  }

  ESP_LOGI(TAG, "%4d %s success", __LINE__, __func__);

  return err;
}

static esp_err_t rs232_PRS3342_parse_yaoce_grp_guiyi(rs232_PRS3342_t *prs3342,
                                                     uint8_t *buf, int size,
                                                     int grp_addr,
                                                     int grp_size) {
  if (size != 3 * grp_size) {
    ESP_LOGE(TAG, "%4d %s want size %d but get %d", __LINE__, __func__,
             3 * grp_size, size);
    return ESP_ERR_INVALID_ARG;
  }

  for (int grp_index = 0; grp_index < grp_size; grp_index++) {
    int match = 0;
    int grp_data = 0;
    int grp_type = 0;

    grp_data = (int)buf[3 * grp_index] | (int)buf[3 * grp_index + 1] << 8;
    grp_type = (int)buf[3 * grp_index + 2];

    if (grp_type == 1) {
      grp_data = 0;
    }

    for (int i = 0; i < prs3342->data_num; i++) {
      if (prs3342->datas[i]->addr == grp_addr + grp_index) {
        prs3342->datas[i]->data =
            (grp_data / 32768.0) * prs3342->datas[i]->convert;
        prs3342->datas[i]->update_time = esp_timer_get_time() / 1000000;
        match++;
        /*
          ESP_LOGI(TAG, "%4d %s get addr:%4x data:%.2f name:%s", __LINE__,
                   __func__, prs3342->datas[i]->addr, prs3342->datas[i]->data,
                   prs3342->datas[i]->name);*/
      }
    }

    /*
        if (match == 0) {
          ESP_LOGW(TAG, "%4d %s data no match", __LINE__, __func__);
        } else if (match > 1) {
          ESP_LOGW(TAG, "%4d %s match more:%d", __LINE__, __func__, match);
        }*/
  }

  return ESP_OK;
}

static esp_err_t rs232_PRS3342_parse_yaoce(rs232_PRS3342_t *prs3342,
                                           uint8_t *buf, int size) {
  esp_err_t err = ESP_OK;
  int grp_size = 0;
  int grp_addr = (int)buf[13] | (int)buf[14] << 8;

  grp_size = buf[8] & 0x7F;

  switch (buf[7]) {
  case RS232_PRS3342_DATA_TYPE_GUIYI:
    err = rs232_PRS3342_parse_yaoce_grp_guiyi(prs3342, &buf[15], size - 15 - 2,
                                              grp_addr, grp_size);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s rs232_PRS3342_parse_yaoce_grp error", __LINE__,
               __func__);
      return ESP_ERR_INVALID_RESPONSE;
    }
    break;
  default:
    ESP_LOGE(TAG, "%4d %s unexcept type:%2x", __LINE__, __func__, buf[7]);
    return ESP_ERR_INVALID_RESPONSE;
  }

  ESP_LOGI(TAG, "%4d %s success", __LINE__, __func__);

  return err;
}

static esp_err_t rs232_PRS3342_parse_dispatch(rs232_PRS3342_t *prs3342,
                                              uint8_t *buf, int size) {
  esp_err_t err = ESP_OK;
  int start_addr = 0;

  start_addr = (int)buf[13] | (int)buf[14] << 8;

  if (start_addr > 0 && start_addr < 0x4000) {
    err = rs232_PRS3342_parse_yaoxin(prs3342, buf, size);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s rs232_PRS3342_parse_yaoxin error", __LINE__,
               __func__);
      return ESP_ERR_INVALID_RESPONSE;
    }
  } else if (start_addr >= 0x4000) {
    err = rs232_PRS3342_parse_yaoce(prs3342, buf, size);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s rs232_PRS3342_parse_yaoce error", __LINE__,
               __func__);
      return ESP_ERR_INVALID_RESPONSE;
    }
  }

  return err;
}

static esp_err_t rs232_PRS3342_parse(rs232_PRS3342_t *prs3342, uint8_t *buf,
                                     int size) {
  esp_err_t err = ESP_OK;
  int data_size = 0;

  if (size < RS232_PRS3342_DATA_MIN_SIZE) {
    ESP_LOGE(TAG, "%4d %s data size:%d less than %d", __LINE__, __func__, size,
             RS232_PRS3342_DATA_MIN_SIZE);
    return ESP_ERR_INVALID_ARG;
  }

  if (buf[0] != 0x68) {
    ESP_LOGE(TAG, "%4d %s error buf 0 except 0x68 but get 0x%2x", __LINE__,
             __func__, buf[0]);
    return ESP_ERR_INVALID_ARG;
  }

  if (buf[1] != buf[2]) {
    ESP_LOGE(TAG, "%4d %s buf 1 != buf 2", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  data_size = buf[1];

  if (buf[3] != 0x68) {
    ESP_LOGE(TAG, "%4d %s error buf 3 except 0x68 but get %2x", __LINE__,
             __func__, buf[3]);
    return ESP_ERR_INVALID_ARG;
  }

  if (size == data_size + 4 + 2) {
    err = rs232_PRS3342_parse_dispatch(prs3342, buf, size);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s rs232_PRS3342_parse_dispatch error", __LINE__,
               __func__);
      return ESP_ERR_INVALID_RESPONSE;
    }
  } else if (size > data_size + 4 + 2) {
    err = rs232_PRS3342_parse_dispatch(prs3342, buf, data_size + 4 + 2);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s rs232_PRS3342_parse_dispatch error", __LINE__,
               __func__);
      return ESP_ERR_INVALID_RESPONSE;
    }

    err = rs232_PRS3342_parse(prs3342, &buf[data_size + 4 + 2],
                              size - (data_size + 4 + 2));
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s rs232_PRS3342_parse error", __LINE__, __func__);
      return ESP_ERR_INVALID_RESPONSE;
    }
  } else {
    ESP_LOGE(TAG, "%4d %s get want size:%d but get size:%d", __LINE__, __func__,
             data_size + 4 + 2, size);
    return ESP_ERR_INVALID_RESPONSE;
  }

  return err;
}

static void rs232_PRS3342_loop(rs232_PRS3342_t *prs3342) {
  while (true) {
    int read_size = 0;
    uint8_t *read_buf = NULL;

    read_buf = rs232_dev_read(prs3342->rs232_config, &read_size);
    if (read_size > 0) {
      ESP_LOGI(TAG, "%4d %s rs232_dev_read, size=%d", __LINE__, __func__,
               read_size);
      for (int i = 0; i < read_size; i++) {
        printf("%2x ", read_buf[i]);
      }
      printf("\n");

      rs232_PRS3342_parse(prs3342, read_buf, read_size);
    }

    if (read_buf != NULL)
      free(read_buf);
  }
}

// global func
// ================================================================

esp_err_t rs232_PRS3342_task(rs232_PRS3342_t *prs3342) {
  esp_err_t err = ESP_OK;

  if (prs3342->rs232_config == NULL) {
    ESP_LOGE(TAG, "%4d %s rs232_config NULL", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  err = rs232_dev_init(prs3342->rs232_config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s rs232_dev_init failed", __LINE__, __func__);
    return ESP_ERR_INVALID_STATE;
  }

  xTaskCreate((TaskFunction_t)rs232_PRS3342_loop, "PRS3342_TASK", 8 * 1024,
              prs3342, 10, NULL);

  return ESP_OK;
}

esp_err_t rs232_PRS3342_new_guiyi(rs232_PRS3342_data_t *data, int addr,
                                  float convert, char *name) {
  data->addr = addr;
  data->type = RS232_PRS3342_DATA_TYPE_GUIYI;
  data->convert = convert;
  data->name = name;
  data->update_time = 0;

  return ESP_OK;
}

esp_err_t rs232_PRS3342_set_context(rs232_PRS3342_t *prs3342) {
  PRS3342 = prs3342;

  return ESP_OK;
}

rs232_PRS3342_t *rs232_PRS3342_get_context() { return PRS3342; }
