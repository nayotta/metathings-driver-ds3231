idf_component_register(SRCS "mt_module_flow.c"
                       INCLUDE_DIRS "."
                       REQUIRES mt_protobuf_c
                                mt_module_http
                                mt_proto_device_cloud
                       PRIV_REQUIRES    mt_nvs_config 
                                        mt_mqtt
                                        json
                                        mt_module_flow_manage
                                        mt_utils)
