#ifndef _MT_CONFIG_HTTP_SERVER_H
#define _MT_CONFIG_HTTP_SERVER_H

#include "esp_http_server.h"

// func =======================================================================

httpd_handle_t mt_config_http_server_start_webserver();

void mt_config_http_server_stop_webserver(httpd_handle_t server);

#endif
