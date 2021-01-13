idf_component_register(SRCS gpio_maglock001_module_flow.c
                            gpio_maglock001_module_mqtt.c
                            gpio_maglock001.c
                       INCLUDE_DIRS .
                       REQUIRES mt_module_flow
                       PRIV_REQUIRES    mt_nvs_storage 
                                        mt_nvs_config
                                        mt_module_http
                                        mt_module_mqtt 
                                        mt_proto_maglock
                                        mt_protobuf_c
                                        mt_proto_device_cloud
                                        mqtt
                                        mt_module_flow_manage
                                        mt_module_unarycall
                                        mt_mqtt_lan)
