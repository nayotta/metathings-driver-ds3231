idf_component_register(SRCS mt_sntp.c
                       INCLUDE_DIRS .
                       REQUIRES 
                       PRIV_REQUIRES    mt_nvs_storage 
                                        mt_smartconfig 
                                        mt_rtc_time )
