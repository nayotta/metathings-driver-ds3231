idf_component_register(SRCS mt_network.c
                       INCLUDE_DIRS .
                       REQUIRES mt_smartconfig
                                mt_network
                       PRIV_REQUIRES    mt_nvs_config 
                                        mt_ethernet)
