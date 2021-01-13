idf_component_register(SRCS any.pb-c.c
                            empty.pb-c.c
                            protobuf-c.c
                            struct.pb-c.c
                            timestamp.pb-c.c
                            wrappers.pb-c.c
                       INCLUDE_DIRS include/google/protobuf 
                                    include/protobuf-c
                                    include
                       REQUIRES 
                       PRIV_REQUIRES)
