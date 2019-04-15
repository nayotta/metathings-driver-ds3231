#include "esp_log.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "mt_nvs_storage.h"

static const char *TAG = "MT_NVS_STORAGE_EXAMPLE";

char *label_int32 = "int32_test";
char *label_string = "string_test";
char *label_byte = "byte_test";

int32_t in_int32 = 5;
char *in_string = "test string";
int in_byte_len = 5;
uint8_t in_byte[5] = {0x31, 0x32, 0x33, 0x34, 0x35};

void app_main() {
  int32_t ret_int32 = 0;
  char *ret_string = NULL;
  size_t ret_string_len = 0;
  uint8_t *ret_byte = NULL;
  int ret_byte_len = 0;

  ESP_LOGI(TAG, "NVS_STORAGE_TEST");

  // int32 write
  if (mt_nvs_write_int32_config(label_int32, in_int32) == true) {
    ESP_LOGI(TAG, "success mt_nvs_write_int32_config: [%d]", in_int32);
  } else {
    ESP_LOGE(TAG, "failed mt_nvs_write_int32_config");
  }

  // int32 read
  if (mt_nvs_read_int32_config(label_int32, &ret_int32) == true) {
    if (in_int32 != ret_int32) {
      ESP_LOGE(TAG, "failed mt_nvs_read_int32_config not equal: [%d] != [%d]",
               in_int32, ret_int32);
    } else {
      ESP_LOGI(TAG, "success mt_nvs_read_int32_config: [%s]=[%d]", label_int32,
               ret_int32);
    }
  } else {
    ESP_LOGE(TAG, " failed mt_nvs_read_int32_config");
  }

  // string write
  if (mt_nvs_write_string_config(label_string, in_string) == true) {
    ESP_LOGI(TAG, "success mt_nvs_write_string_config [%s]", in_string);
  } else {
    ESP_LOGE(TAG, "mt_nvs_write_string_config error");
  }

  // string read
  ret_string = mt_nvs_read_string_config(label_string, &ret_string_len);
  if (ret_string_len > 0) {
    if (strcmp(in_string, ret_string) != 0) {
      ESP_LOGE(TAG, "failed mt_nvs_read_string_config not euqal:[%s] != [%s]",
               in_string, ret_string);
    } else {
      ESP_LOGI(TAG, "success READ str [%s]=[%s]", in_string, ret_string);
    }
  } else {
    ESP_LOGE(TAG, "failed mt_nvs_read_string_config");
  }

  // byte write
  if (mt_nvs_write_byte_config(label_byte, in_byte_len, in_byte) == true) {
    ESP_LOGI(TAG, "success mt_nvs_write_byte_config [%d] bytes", in_byte_len);
  } else {
    ESP_LOGE(TAG, "mt_nvs_write_byte_config error");
  }

  // byte read
  ret_byte = mt_nvs_read_byte_config(label_byte, &ret_byte_len);
  if (ret_byte == NULL) {
    ESP_LOGE(TAG, "mt_nvs_read_byte_config error");
    goto BYTE_NEXT;
  } else {
    if (ret_byte_len != in_byte_len) {
      ESP_LOGE(TAG, "mt_nvs_read_byte_config error len not equal: [%d] != [%d]",
               ret_byte_len, in_byte_len);
      goto BYTE_NEXT;
    } else {
      for (int i = 0; i < ret_byte_len; i++) {
        if (ret_byte[i] != in_byte[i]) {
          ESP_LOGE(
              TAG,
              "mt_nvs_read_byte_config error value not equal: [%d] != [%d]",
              ret_byte[i], in_byte[i]);
          goto BYTE_NEXT;
        }
      }
    }
  }

  ESP_LOGI(TAG, "success mt_nvs_read_byte_config");
BYTE_NEXT:

  ESP_LOGI(TAG, "mt nvs_storage test over");
}
