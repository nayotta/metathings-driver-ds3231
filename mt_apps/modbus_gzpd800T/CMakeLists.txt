idf_component_register(SRCS modbus_gzpd800T_module_flow.c
                            modbus_gzpd800T.c
                       INCLUDE_DIRS .
                       REQUIRES mt_module_flow 
                                mt_modbus_master
                       PRIV_REQUIRES    mt_mqtt_lan
                                        mt_proto_device_cloud
                                        mt_protobuf_c
                                        mt_module_mqtt
                                        mt_module_http)
