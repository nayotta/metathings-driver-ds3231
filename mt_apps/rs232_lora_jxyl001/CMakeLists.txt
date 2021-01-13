idf_component_register(SRCS rs232_lora_jxyl001_module_flow.c
                            rs232_lora_jxyl001.c
                       INCLUDE_DIRS .
                       REQUIRES mt_module_flow
                                rs232_dev
                       PRIV_REQUIRES    mt_nvs_storage 
                                        mt_module_http
                                        mt_mqtt
                                        mt_protobuf_c
                                        mt_proto_device_cloud)
