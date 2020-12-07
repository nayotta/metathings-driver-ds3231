#ifndef _HUAWEI_MQTT_UTILS_H
#define _HUAWEI_MQTT_UTILS_H

#include "cJSON.h"
#include "esp_err.h"
#include "esp_log.h"
#include "string.h"

// define =====================================================================

typedef struct _huawei_cmd_t {
  char *msg_type;
  int mid;
  char *method;
  cJSON *paras;
  char *serviceID;
  char *clientID;
} huawei_cmd_t;

// help func ==================================================================

huawei_cmd_t *huawei_mqtt_utils_new_cmd_t();

void huawei_mqtt_utils_free_cmd_t(huawei_cmd_t *cmd_t);

// func =======================================================================

char *huawei_mqtt_utils_gen_push(cJSON *data, char *clientID, char *serviceID);

char *huawei_mqtt_utils_gen_resp(cJSON *res, char *state, int errcode, int mid);

huawei_cmd_t *huawei_mqtt_utils_parse_cmd(char *msg,int size);

#endif
