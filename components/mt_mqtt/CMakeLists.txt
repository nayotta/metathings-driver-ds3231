idf_component_register(SRCS mt_mqtt_lan.c
                            mt_mqtt_utils.c
                            mt_mqtt.c
                       INCLUDE_DIRS "."
                       REQUIRES mqtt
                       PRIV_REQUIRES    rs232_sim_air720h
                                        mt_nvs_storage
                                        mt_nvs_config
                                        nvs_flash
                                        mt_utils)
