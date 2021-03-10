idf_component_register(SRCS modbus_ktc_n01.c
                       INCLUDE_DIRS "."
                       REQUIRES mt_modbus_master
                       PRIV_REQUIRES    mt_nvs_storage
                                        mt_utils
                                        json)
