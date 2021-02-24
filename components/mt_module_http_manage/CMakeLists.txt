idf_component_register(SRCS "mt_module_http_manage.c"
                       INCLUDE_DIRS "."
                       REQUIRES mt_module_http
                       PRIV_REQUIRES    mt_module_flow  
                                        mt_module_mqtt
                                        mt_mqtt)
