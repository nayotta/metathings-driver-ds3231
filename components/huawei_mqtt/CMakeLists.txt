idf_component_register(SRCS huawei_mqtt_manage.c
                            huawei_mqtt_utils.c
                            huawei_mqtt.c
                       INCLUDE_DIRS .
                       REQUIRES 
                       PRIV_REQUIRES    mqtt 
                                        json 
                                        mt_mqtt_lan 
                                        rs232_sim_air720h
                                        mt_utils
                                        mt_nvs_config)
