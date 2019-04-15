# metathings-esp32-lib
lib for esp32

## 参考

esp32官方 [ESP-IDF framework](https://github.com/espressif/esp-idf)

参考来源 [esp-open-rtos](https://github.com/SuperHouse/esp-open-rtos)

参考来源 [esp-idf-lib](https://github.com/UncleRus/esp-idf-lib)


## 模块

| 模块              | 描述                                                                     | 授权 | 线程安全|
|-------------------|-------------------------------------------------------------------------|------|--------|
| **cryptolib**     | cryptoauthlib 基础库                                                    | GNU   | 是     |
| **cryptolibhal**  | cryptoauthlib hal 对接esp32                                             | GNU   | -      |
| **i2c_ds1307**    | 硬件I2C时钟模块                                                          | MIT  | 是     |
| **i2c_ds3231**    | 硬件I2C时钟模块附带温度测量                                               | MIT  | 是     |
| **i2c_atecc608a** | 硬件I2C加密芯片,安全密钥存储器                                            | MIT  | -      |
| **i2cdev**        | ESP32 i2c driver                                                        | GNU  | -      |
