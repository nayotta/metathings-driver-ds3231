idf_component_register(SRCS "mt_module_http_utils.c"
                            "mt_module_http.c"
                       INCLUDE_DIRS "."
                       REQUIRES mt_http_client
                                mt_module_http
                                esp_http_client
                       PRIV_REQUIRES    mt_utils 
                                        json 
                                        mt_nvs_config  
                                        mt_module_http_manage
                                        mt_mqtt
                                        mt_mqtt_lan)
