idf_component_register(SRCS  rs232_sim_air720h_flow.c
                             rs232_sim_air720h_http.c
                             rs232_sim_air720h_mqtt.c
                             rs232_sim_air720h_recv_manage_http_action.c
                             rs232_sim_air720h_recv_manage_http_read.c
                             rs232_sim_air720h_recv_manage_mqtt_state.c
                             rs232_sim_air720h_recv_manage_mqtt_sub.c
                             rs232_sim_air720h_recv_manage.c
                             rs232_sim_air720h_sent_manage.c
                             rs232_sim_air720h_utils.c
                             rs232_sim_air720h.c
                       INCLUDE_DIRS .
                       REQUIRES rs232_dev  
                                mt_module_flow 
                                mt_module_http
                                mt_module_mqtt
                       PRIV_REQUIRES    mt_nvs_config 
                                        mt_module_lora
                                        mt_mqtt
                                        mt_utils)
