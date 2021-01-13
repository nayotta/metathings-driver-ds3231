idf_component_register(SRCS "mt_config_http_server.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_http_server
                       PRIV_REQUIRES mt_nvs_config)
