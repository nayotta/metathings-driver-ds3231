idf_component_register(SRCS "mt_ethernet.c"
                       INCLUDE_DIRS "."
                       REQUIRES 
                       PRIV_REQUIRES    gpio_light 
                                        nvs_flash)
