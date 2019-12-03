#ifndef _MT_JSON_PARSE_H
#define _MT_JSON_PARSE_H

#include "esp_err.h"
#include "string.h"

typedef struct _mt_json_field_t {
  char *field;
  int key_size;
  char **key_value;
} mt_json_field_t;

typedef struct _mt_json_t {
  char *flow_name;
  int field_size;
  mt_json_field_t **fileds;
} mt_json_t;

uint8_t *mt_json_parse(char *buf_in);


#endif
