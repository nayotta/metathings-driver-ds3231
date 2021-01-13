idf_component_register(SRCS "mt_module_flow_manage.c"
                       INCLUDE_DIRS "."
                       REQUIRES mt_module_flow
                       PRIV_REQUIRES    mt_module_http 
                                        mt_module_mqtt 
                                        mt_mqtt_lan 
                                        mt_mqtt)
