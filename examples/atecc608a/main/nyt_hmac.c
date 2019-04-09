#include "stdio.h"

#include "basic/atca_basic.h"

static const uint8_t hmac_data[5] = {0x31, 0x32, 0x33, 0x34, 0x35};
/*static const uint8_t g_access_key[32] = {
    0x31, 0x12, 0xd0, 0x66, 0xf5, 0xed, 0x52, 0xc7, 0x79, 0x98, 0xff,
    0xaa, 0xac, 0x43, 0x22, 0x60, 0xdd, 0xff, 0x9c, 0x10, 0x99, 0x6f,
    0x41, 0x66, 0x3a, 0x60, 0x23, 0xfa, 0xf6, 0xaa, 0x3e, 0xc5};*/
static const uint8_t g_null_key[32] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static const uint8_t g_access_key[4] = {0x35, 0x12, 0xd0, 0x66};

// hmac use 32byte key restrict, if below use 0xff pad
int nyt_hmac()
{
    int ret = 0;
    uint8_t hmac_out[32];
    uint8_t key_out[64];
    int test_slot = 12;

    ret = atcab_write_zone(ATCA_ZONE_DATA, test_slot, 0, 0, g_null_key, 32);
    if (ret != ATCA_SUCCESS)
    {
        printf("ERROR:refresh test_slot ret=%d\n", ret);
        return ret;
    }

    ret = atcab_write_zone(ATCA_ZONE_DATA, test_slot, 0, 0, g_access_key, 4);
    if (ret != ATCA_SUCCESS)
    {
        printf("ERROR:atcab_write_zone ret=%d\n", ret);
        return ret;
    }

    ret = atcab_read_zone(ATCA_ZONE_DATA, test_slot, 0, 0, key_out, 32);
    if (ret != ATCA_SUCCESS)
    {
        printf("ERROR:atcab_read_zone ret=%d\n", ret);
        //return ret;
    }

    ret = atcab_read_zone(ATCA_ZONE_DATA, test_slot, 1, 0, &key_out[32], 32);
    if (ret != ATCA_SUCCESS)
    {
        printf("ERROR:atcab_read_zone ret=%d\n", ret);
        //return ret;
    }
    for (int i = 0; i < 64; i++)
    {
        printf("%2x ", key_out[i]);
    }
    printf("\n");

    ret = atcab_sha_hmac(hmac_data, 5, test_slot, hmac_out,
                         SHA_MODE_TARGET_TEMPKEY);
    if (ret != ATCA_SUCCESS)
    {
        printf("ERROR:atcab_sha_hmac ret=%d\n", ret);
        return ret;
    }

    printf("hmac is-------->\n");
    for (int i = 0; i < 32; i++)
    {
        printf("%2x ", hmac_out[i]);
    }
    printf("\n");
    return ret;
}