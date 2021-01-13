idf_component_register(SRCS "."
                       INCLUDE_DIRS "."
                       REQUIRES mt_module_flow 
                                mt_modbus_master
                                mt_proto_ktc_n01
                       PRIV_REQUIRES    huawei_mqtt
                                        mt_nvs_config
                                        mt_utils
                                        mt_mqtt
                                        mt_module_http
                                        mt_module_flow_manage
                                        mt_protobuf_c
                                        mt_proto_device_cloud)
