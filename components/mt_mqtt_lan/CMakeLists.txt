idf_component_register(SRCS mt_mqtt_lan.c
                       INCLUDE_DIRS .
                       REQUIRES mqtt
                       PRIV_REQUIRES    mt_mqtt 
                                        mt_mqtt_lan 
                                        mt_nvs_storage 
                                        mt_nvs_config 
                                        nvs_flash 
                                        mt_utils)
