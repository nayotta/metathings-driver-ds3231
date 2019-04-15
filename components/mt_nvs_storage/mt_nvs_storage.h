#include "esp_system.h"
#include "nvs.h"
#include "stdio.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

nvs_handle mt_nvs_get_handle();
bool mt_nvs_init();

uint8_t *mt_nvs_read_byte_config(char *config_name, int *len);
char *mt_nvs_read_string_config(char *config_name, size_t *len);
bool mt_nvs_read_int32_config(char *config_name, int32_t *config_value);

bool mt_nvs_write_byte_config(char *config_name, int len, uint8_t *buf);
bool mt_nvs_write_string_config(char *config_name, char *config_value);
bool mt_nvs_write_int32_config(char *config_name, int32_t config_value);
