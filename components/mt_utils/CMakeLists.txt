idf_component_register(SRCS mt_utils_base64.c
                            mt_utils_byte.c
                            mt_utils_login.c
                            mt_utils_session.c
                            mt_utils_string.c
                            mt_utils.c
                       INCLUDE_DIRS .
                       REQUIRES 
                       PRIV_REQUIRES    json
                                        mbedtls)
