idf_component_register(SRCS modbus_bits001_huawei_flow.c
                            modbus_bits001_module_flow.c
                            modbus_bits001.c
                       INCLUDE_DIRS .
                       REQUIRES mt_module_flow 
                                mt_modbus_master
                                json
                                mt_nvs_config
                       PRIV_REQUIRES    mt_mqtt
                                        mt_utils
                                        mt_module_mqtt
                                        mt_module_http
                                        mt_proto_device_cloud
                                        mt_protobuf_c
                                        huawei_iot)
