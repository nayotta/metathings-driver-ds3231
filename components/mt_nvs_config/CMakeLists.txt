idf_component_register(SRCS mt_nvs_config.c
                       INCLUDE_DIRS .
                       REQUIRES json
                       PRIV_REQUIRES    mt_nvs_storage 
                                        json 
                                        mt_utils)
