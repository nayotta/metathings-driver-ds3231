idf_component_register(SRCS "mt_module_lora.c"
                       INCLUDE_DIRS "."
                       REQUIRES rs232_lora_ebyte
                        PRIV_REQUIRES   mt_module_flow
                                        mt_mqtt 
                                        mt_protobuf_c 
                                        mt_proto_device_cloud 
                                        mt_module_mqtt 
                                        mt_module_http_manage
                                        mt_nvs_storage 
                                        mt_nvs_config
                                        mt_utils
                                        mt_module_unarycall)
