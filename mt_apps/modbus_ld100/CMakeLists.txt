idf_component_register(SRCS modbus_ld100.c
                       INCLUDE_DIRS .
                       REQUIRES mt_modbus_master
                       PRIV_REQUIRES json)
