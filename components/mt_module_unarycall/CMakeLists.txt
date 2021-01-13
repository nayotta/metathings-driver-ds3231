idf_component_register(SRCS mt_module_unarycall_config.c
                            mt_module_unarycall_monitor.c
                            mt_module_unarycall_ota.c
                            mt_module_unarycall.c
                            mt_module_unarycall_utils.c
                       INCLUDE_DIRS .
                       REQUIRES mt_protobuf_c 
                                mt_proto_device_cloud 
                       PRIV_REQUIRES    mt_mqtt_lan
                                        mt_mqtt
                                        mt_utils
                                        mt_ota
                                        esp_https_ota
                                        app_update
                                        mt_sys_config
                                        mt_sys_monitor
                                        mt_proto_sys_config
                                        mt_proto_sys_monitor
                                        mt_proto_sys_ota)
