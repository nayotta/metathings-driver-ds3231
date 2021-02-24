idf_component_register(SRCS modbus_ld100_module_flow.c
                            modbus_ld100.c
                       INCLUDE_DIRS .
                       REQUIRES mt_module_flow 
                                mt_modbus_master
                       PRIV_REQUIRES    mt_module_http
                                        mt_mqtt
                                        mt_utils
                                        mt_protobuf_c
                                        mt_proto_device_cloud)
