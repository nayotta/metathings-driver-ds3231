#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "rs232_lora_ebyte_unarycall_manage.h"

// static define ==============================================================

static const char *TAG = "RS232_LORA_EBYTE_UNARYCALL_MANAGE";
static rs232_lora_ebyte_unarycall_manage_t *UNARYCALL_MANAGE = NULL;
static SemaphoreHandle_t SemaphorMasterHdl = NULL;
static int Lock_Timeout = 3000;

// static func ================================================================

static void lock_init() {
  SemaphorMasterHdl = xSemaphoreCreateMutex();
  return;
}

static bool lock_take(int32_t timeout) {
  if (SemaphorMasterHdl == NULL) {
    lock_init();
  }
  if (xSemaphoreTake(SemaphorMasterHdl, (portTickType)timeout) == pdTRUE) {
    return true;
  }
  return false;
}

static void lock_release() {
  if (SemaphorMasterHdl == NULL) {
    lock_init();
  }
  xSemaphoreGive(SemaphorMasterHdl);
  return;
}

// global func ================================================================

esp_err_t rs232_lora_ebyte_unarycall_manage_add(QueueHandle_t handle) {
  esp_err_t err = ESP_OK;
  if (lock_take(Lock_Timeout) == false) {
    ESP_LOGE(TAG, "%4d %s lock timeout", __LINE__, __func__);
    return ESP_ERR_TIMEOUT;
  }

  rs232_lora_ebyte_unarycall_manage_t *temp_manage = NULL;

  if (UNARYCALL_MANAGE == NULL) {
    UNARYCALL_MANAGE = malloc(sizeof(rs232_lora_ebyte_unarycall_manage_t));
    UNARYCALL_MANAGE->unarycall_size = 1;
    UNARYCALL_MANAGE->unarycall_handle =
        malloc(sizeof(rs232_lora_ebyte_unarycall_t *));
    UNARYCALL_MANAGE->unarycall_handle[0] =
        malloc(sizeof(rs232_lora_ebyte_unarycall_t));
    UNARYCALL_MANAGE->unarycall_handle[0]->handle = handle;
    goto EXIT;
  }

  temp_manage = UNARYCALL_MANAGE;
  UNARYCALL_MANAGE = malloc(sizeof(rs232_lora_ebyte_unarycall_manage_t));
  UNARYCALL_MANAGE->unarycall_size = temp_manage->unarycall_size + 1;
  UNARYCALL_MANAGE->unarycall_handle =
      malloc(UNARYCALL_MANAGE->unarycall_size *
             (sizeof(rs232_lora_ebyte_unarycall_t *)));
  for (int i = 0; i < UNARYCALL_MANAGE->unarycall_size - 1; i++) {
    UNARYCALL_MANAGE->unarycall_handle[i] = temp_manage->unarycall_handle[i];
  }
  UNARYCALL_MANAGE->unarycall_handle[UNARYCALL_MANAGE->unarycall_size - 1] =
      malloc(sizeof(rs232_lora_ebyte_unarycall_t));
  UNARYCALL_MANAGE->unarycall_handle[UNARYCALL_MANAGE->unarycall_size - 1]
      ->handle = handle;
  UNARYCALL_MANAGE->unarycall_handle[UNARYCALL_MANAGE->unarycall_size - 1]
      ->create_time = esp_timer_get_time() / 1000000;

EXIT:
  lock_release();
  return err;
}

esp_err_t rs232_lora_ebyte_unarycall_manage_del(QueueHandle_t handle) {
  esp_err_t err = ESP_OK;
  int match = 0;
  rs232_lora_ebyte_unarycall_manage_t *temp_manage = NULL;

  if (lock_take(Lock_Timeout) == false) {
    ESP_LOGE(TAG, "%4d %s lock timeout", __LINE__, __func__);
    return ESP_ERR_TIMEOUT;
  }

  if (UNARYCALL_MANAGE == NULL) {
    ESP_LOGE(TAG, "%4d %s UNARYCALL_MANAGE NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  for (int i = 0; i < UNARYCALL_MANAGE->unarycall_size; i++) {
    if (UNARYCALL_MANAGE->unarycall_handle[i]->handle == handle) {
      match++;
    }
  }

  if (match > 0) {
    if (UNARYCALL_MANAGE->unarycall_size == match) {
      for (int i = 0; i < UNARYCALL_MANAGE->unarycall_size; i++) {
        vQueueDelete(UNARYCALL_MANAGE->unarycall_handle[i]->handle);
        free(UNARYCALL_MANAGE->unarycall_handle[i]);
      }
      UNARYCALL_MANAGE = NULL;
      goto EXIT;
    } else {
      temp_manage = UNARYCALL_MANAGE;
      UNARYCALL_MANAGE = malloc(sizeof(rs232_lora_ebyte_unarycall_manage_t));
      UNARYCALL_MANAGE->unarycall_size = temp_manage->unarycall_size - match;
      UNARYCALL_MANAGE->unarycall_handle =
          malloc(UNARYCALL_MANAGE->unarycall_size *
                 sizeof(rs232_lora_ebyte_unarycall_t *));
      int new_unarycall_count = 0;

      for (int i = 0; i < temp_manage->unarycall_size; i++) {
        if (temp_manage->unarycall_handle[i]->handle == handle) {
          vQueueDelete(temp_manage->unarycall_handle[i]->handle);
          free(temp_manage->unarycall_handle[i]);
        } else {
          UNARYCALL_MANAGE->unarycall_handle[new_unarycall_count] =
              temp_manage->unarycall_handle[i];
          new_unarycall_count++;
        }
      }

      free(temp_manage);
    }
  }
EXIT:
  lock_release();
  return err;
}
