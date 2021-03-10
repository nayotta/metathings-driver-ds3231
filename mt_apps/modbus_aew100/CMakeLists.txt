idf_component_register(SRCS modbus_aew100.c
                       INCLUDE_DIRS .
                       REQUIRES 
                       PRIV_REQUIRES    json 
                                        mt_nvs_storage
                                        mt_modbus_master
                                        mt_utils)
