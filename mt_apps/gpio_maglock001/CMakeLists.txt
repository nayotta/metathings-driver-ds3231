idf_component_register(SRCS gpio_maglock001.c
                       INCLUDE_DIRS .
                       REQUIRES mt_module_flow
                       PRIV_REQUIRES    mt_nvs_storage 
                                        mt_nvs_config
                                        mt_utils)
