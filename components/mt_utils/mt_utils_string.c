#include "string.h"

#include "mt_utils_string.h"

char *mt_utils_string_copy(char *str_in) {
  if (str_in == NULL) {
    return NULL;
  }

  int str_in_size = strlen(str_in) + 1;
  char *str_out = malloc(str_in_size);
  memcpy(str_out, str_in, str_in_size);

  return str_out;
}
