idf_component_register(SRCS mt_rtc_time.c
                       INCLUDE_DIRS .
                       REQUIRES 
                       PRIV_REQUIRES    i2c_ds1307
                                        i2c_ds3231
                                        i2cdev
                                        mt_nvs_storage)
