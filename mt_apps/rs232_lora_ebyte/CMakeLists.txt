idf_component_register(SRCS rs232_lora_ebyte_flow_manage.c
                            rs232_lora_ebyte_module_manage.c
                            rs232_lora_ebyte_unarycall_manage.c
                            rs232_lora_ebyte.c
                       INCLUDE_DIRS .
                       REQUIRES mt_module_flow
                                rs232_dev
                       PRIV_REQUIRES    mt_nvs_storage 
                                        mt_nvs_config
                                        mt_mqtt_lan
                                        mt_module_mqtt)
