idf_component_register(SRCS modbus_reclose001_module_flow.c
                            modbus_reclose001.c
                       INCLUDE_DIRS .
                       REQUIRES mt_module_flow
                       PRIV_REQUIRES    modbus_reclose001 
                                        mt_modbus_master
                                        mt_utils)
