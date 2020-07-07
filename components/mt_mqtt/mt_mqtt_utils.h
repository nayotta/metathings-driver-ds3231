#ifndef _MT_MQTT_UTILS_H
#define _MT_MQTT_UTILS_H

#include "esp_err.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

esp_err_t mt_mqtt_utils_get_session_id_from_topic(char *topic,
                                                  uint64_t *session_id);

char *mt_mqtt_utils_get_session_string_from_topic(char *topic);

char *mt_mqtt_utils_get_path_from_topic(char *topic);

char *mt_mqtt_utils_get_module_id_from_topic(char *topic);

char *mt_mqtt_utils_set_path_downstream_to_upstream(char *topic);

char *mt_mqtt_utils_convert_path_unarycall_req_to_res(char *topic,
                                                      char *session_id);

char *mt_mqtt_utils_new_module_topic(char *module_id);

char *mt_mqtt_utils_new_device_topic(char *device_id);

#endif
