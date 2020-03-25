#ifndef _RS232_SIM_AIR720H_MQTT_H
#define _RS232_SIM_AIR720H_MQTT_H

#include "esp_system.h"

// define =====================================================================

typedef enum {
  RS232_SIM_AIR720H_SENT_MODE_ARSCII = 0,
  RS232_SIM_AIR720H_SENT_MODE_HEX = 1,
} RS232_SIM_AIR720H_SENT_MODE_T;

typedef enum {
  RS232_SIM_AIR720H_RECV_MODE_DIRECT = 0,
  RS232_SIM_AIR720H_RECV_MODE_CACHE = 1,
} RS232_SIM_AIR720H_RECV_MODE_T;

// func =======================================================================

// set config
esp_err_t rs322_sim_air720h_mqtt_set_client_config(char *username,
                                                   char *password);

// set host
esp_err_t rs322_sim_air720h_mqtt_set_host(char *host, char *port);

// set connect
esp_err_t rs322_sim_air720h_mqtt_set_connect(int keepalive_interval);

// set sub topic
esp_err_t rs322_sim_air720h_mqtt_set_sub_topic(char *topic);

// set pub msg
esp_err_t rs322_sim_air720h_mqtt_set_pub_msg(char *topic, char *msg);

// set sent mode
esp_err_t
rs322_sim_air720h_mqtt_set_sent_mode(RS232_SIM_AIR720H_SENT_MODE_T mode);

// set recv mode
esp_err_t
rs322_sim_air720h_mqtt_set_recv_mode(RS232_SIM_AIR720H_RECV_MODE_T mode);

// set mqtt close
esp_err_t rs322_sim_air720h_mqtt_set_mqtt_close();

// set close
esp_err_t rs322_sim_air720h_mqtt_set_close();

#endif
