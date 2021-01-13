idf_component_register(SRCS modbus_aew100_module_flow.c
                            modbus_aew100_module_mqtt.c
                            modbus_aew100_utils.c
                            modbus_aew100.c
                       INCLUDE_DIRS .
                       REQUIRES mt_proto_aew100 
                                mt_module_flow
                       PRIV_REQUIRES    mt_modbus_master  
                                        mt_proto_device_cloud
                                        mt_protobuf_c 
                                        mt_utils 
                                        mt_nvs_storage 
                                        mt_module_mqtt
                                        mt_mqtt_lan
                                        mt_module_flow_manage
                                        mt_module_unarycall)
