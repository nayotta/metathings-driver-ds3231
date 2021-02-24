idf_component_register(SRCS rs232_charge001_lora_flow.c
                            rs232_charge001_lora_unarycall.c
                            rs232_charge001_module_flow.c
                            rs232_charge001_module_mqtt.c
                            rs232_charge001_recv_manage_get_state.c
                            rs232_charge001_recv_manage_get_states.c
                            rs232_charge001_recv_manage_notify_coin.c
                            rs232_charge001_recv_manage_notify_over.c
                            rs232_charge001_recv_manage_set_charge.c
                            rs232_charge001_recv_manage_set_stop.c
                            rs232_charge001_recv_manage.c
                            rs232_charge001_sent_manage.c
                            rs232_charge001_utils.c
                            rs232_charge001.c
                       INCLUDE_DIRS .
                       REQUIRES mt_module_flow
                                rs232_dev
                                mt_proto_charge001
                       PRIV_REQUIRES    mt_nvs_config 
                                        rs232_lora_ebyte 
                                        rs232_sim_air720h
                                        mt_device_type 
                                        mt_module_lora
                                        mt_module_unarycall
                                        mt_module_flow_manage
                                        mt_utils)
