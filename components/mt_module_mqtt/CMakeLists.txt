idf_component_register(SRCS "mt_module_mqtt.c"
                       INCLUDE_DIRS "."
                       REQUIRES mt_proto_device_cloud
                                mt_mqtt
                       PRIV_REQUIRES    mt_module_flow 
                                        mt_module_unarycall  
                                        mt_protobuf_c)
