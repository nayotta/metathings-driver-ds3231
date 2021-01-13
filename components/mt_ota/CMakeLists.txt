idf_component_register(SRCS mt_ota.c
                       INCLUDE_DIRS .
                       REQUIRES 
                       PRIV_REQUIRES    esp_http_client
                                        esp_https_ota )
