idf_component_register(SRCS mt_smartconfig.c
                       INCLUDE_DIRS .
                       REQUIRES gpio_light
                       PRIV_REQUIRES    mt_nvs_storage 
                                        tcpip_adapter 
                                        esp_wifi 
                                        gpio_button )
