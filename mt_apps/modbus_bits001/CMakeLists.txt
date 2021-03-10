idf_component_register(SRCS modbus_bits001.c
                       INCLUDE_DIRS "."
                       REQUIRES mt_modbus_master
                                json
                                mt_nvs_storage
                       PRIV_REQUIRES    )
