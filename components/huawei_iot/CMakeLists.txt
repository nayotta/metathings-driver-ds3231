idf_component_register(SRCS huawei_mqtt_flow.c
                            huawei_mqtt_manage.c
                            huawei_mqtt_utils.c
                            huawei_mqtt.c
                       INCLUDE_DIRS .
                       REQUIRES json
                                mt_nvs_config
                       PRIV_REQUIRES    mqtt 
                                        mt_mqtt
                                        rs232_sim_air720h
                                        mt_utils)
