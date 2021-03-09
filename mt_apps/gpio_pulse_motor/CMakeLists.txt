idf_component_register(SRCS gpio_pulse_motor_button.c
                            gpio_pulse_motor_light.c
                            gpio_pulse_motor_module_flow.c
                            gpio_pulse_motor_module_mqtt.c
                            gpio_pulse_motor_utils.c
                            gpio_pulse_motor.c
                       INCLUDE_DIRS .
                       REQUIRES mt_module_flow 
                                mt_proto_pulse_motor
                       PRIV_REQUIRES    mt_nvs_config
                                        mt_module_mqtt
                                        mt_mqtt
                                        gpio_button 
                                        gpio_light
                                        mt_utils
                                        mt_module_unarycall
                                        mt_proto_device_cloud)
