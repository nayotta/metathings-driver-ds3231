#ifndef _MT_UTILS_BASE64_H
#define _MT_UTILS_BASE64_H

#define BASE64_ENCODE_OUT_SIZE(s) ((unsigned int)((((s) + 2) / 3) * 4 + 1))
#define BASE64_DECODE_OUT_SIZE(s) ((unsigned int)(((s) / 4) * 3))

unsigned int mt_utils_base64_encode(const unsigned char *in, unsigned int inlen,
                                    char *out);

unsigned int mt_utils_base64_decode(const char *in, unsigned int inlen,
                                    unsigned char *out);

#endif
