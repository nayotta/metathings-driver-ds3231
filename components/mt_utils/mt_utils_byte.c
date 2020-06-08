#include "mt_utils_byte.h"

int32_t mt_utils_byte_2char_to_int32(uint8_t *byte_in) {
  int32_t num = 0;
  num = byte_in[0] * (1 << 8) + byte_in[1];

  return num;
}

int32_t mt_utils_byte_4char_to_int32(uint8_t *byte_in) {
  int32_t num = 0;
  num = byte_in[0] * (1 << 24) + byte_in[1] * (1 << 16) +
        byte_in[2] * (1 << 8) + byte_in[3];

  return num;
}
