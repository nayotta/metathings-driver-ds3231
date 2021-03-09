idf_component_register(SRCS cache_airswitch001.c
                            modbus_airswitch001_module_flow.c
                            modbus_airswitch001_module_mqtt.c
                            modbus_airswitch001_notify.c
                            modbus_airswitch001_utils.c
                            modbus_airswitch001.c
                       INCLUDE_DIRS .
                       REQUIRES mt_modbus_master_airswitch001 
                                mt_proto_airswitch001
                                mt_module_flow 
                                mt_module_http
                       PRIV_REQUIRES    mt_nvs_storage
                                        mqtt
                                        mt_mqtt
                                        mt_protobuf_c
                                        mt_proto_device_cloud
                                        mt_module_mqtt
                                        mt_module_unarycall)
