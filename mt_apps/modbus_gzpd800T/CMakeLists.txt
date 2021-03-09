idf_component_register(SRCS modbus_gzpd800T.c
                       INCLUDE_DIRS .
                       REQUIRES mt_modbus_master
                       PRIV_REQUIRES json)
