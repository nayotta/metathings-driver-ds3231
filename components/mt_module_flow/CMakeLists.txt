idf_component_register(SRCS "mt_module_flow.c"
                            "mt_module_flow_manage.c"
                       INCLUDE_DIRS "."
                       REQUIRES mt_protobuf_c
                                mt_module_http
                                mt_module_mqtt
                                mt_proto_device_cloud
                                json
                       PRIV_REQUIRES    mt_nvs_config 
                                        mt_mqtt
                                        mt_utils)
