#ifndef HXD019_I2C_H
#define HXD019_I2C_H

#include "esp_err.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

esp_err_t mt_i2c_hxd019_i2c_init();

esp_err_t mt_i2c_hxd019_i2c_learn();

#endif