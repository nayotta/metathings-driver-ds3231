idf_component_register(SRCS mt_mqtt_utils.c
                            mt_mqtt.c
                       INCLUDE_DIRS "."
                       REQUIRES 
                       PRIV_REQUIRES    rs232_sim_air720h
                                        mqtt  
                                        mt_mqtt_lan
                                        mt_nvs_config)
