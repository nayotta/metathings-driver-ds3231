idf_component_register(SRCS modbus_power001_lora_flow.c
                            modbus_power001_storage.c
                            modbus_power001.c
                       INCLUDE_DIRS .
                       PRIV_REQUIRES    mt_nvs_storage 
                                        rs232_lora_ebyte 
                                        mt_device_type 
                                        mt_module_lora
                                        mt_module_flow
                                        mt_modbus_master)
