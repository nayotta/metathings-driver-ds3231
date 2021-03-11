idf_component_register(SRCS cache_airswitch001.c
                            modbus_airswitch001.c
                       INCLUDE_DIRS .
                       REQUIRES 
                       PRIV_REQUIRES    mt_nvs_storage
                                        mt_modbus_master_airswitch001
                                        json
                                        mt_utils)
