idf_component_register(SRCS rs232_EA900II_module_flow.c
                            rs232_EA900II_module_mqtt.c
                            rs232_EA900II.c
                       INCLUDE_DIRS .
                       REQUIRES mt_module_flow
                                rs232_dev
                                mt_module_http
                       PRIV_REQUIRES    mt_proto_EA900II
                                        mt_utils
                                        mt_nvs_storage
                                        mt_mqtt_lan
                                        mt_module_mqtt
                                        mt_module_unarycall
                                        mt_module_flow_manage
                                        mt_protobuf_c
                                        mt_proto_device_cloud)
