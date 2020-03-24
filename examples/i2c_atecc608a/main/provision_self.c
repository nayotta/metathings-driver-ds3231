#include "stdio.h"

#include "basic/atca_basic.h"
#include "provision_self.h"

#define I2C_ADDR 0xC0

static const int g_device_priv_key_slot = 0;
static const int g_signer_priv_key_slot = 2;
static const int g_access_key_slot = 4;
static const int g_signer_ca_priv_key_slot = 7;

static const uint8_t g_ecc_configdata[128] = {
    //< sn[0:3]          >  < rev num            >  < sn[4:8]
    0x01, 0x23, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x04, 0x05, 0x06, 0x07,
    //  > < no> <I2c>       <addr>          <otp> <chp> < slot 0 >  < slot 1 >
    0xEE, 0x00, 0x01, 0x00, I2C_ADDR, 0x00, 0x55, 0x00, 0x8F, 0x20, 0xC4, 0x44,
    //<slot 2>  < slot 3 >  < slot4  >  < slot 5 >  < slot 6 >  < slot 7 >
    0x87, 0x20, 0xC4, 0x44, 0x8F, 0x0F, 0x8F, 0x8F, 0x9F, 0x8F, 0x83, 0x64,
    0xC4, 0x44, 0xC4, 0x44, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    //                      <-count 0                                   ->
    0x0F, 0x0F, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
    //<-count 1                                 ->  <- last key use
    0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
    //                                                                  ->
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    // ex sele  lockD lockC <- lockS -> < none   -> <- x509 format      ->
    0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //< key 0>  < key 1   > < key 2  >  < key 3  >  < key 4  >  < key 5  >
    0x33, 0x00, 0x1C, 0x00, 0x13, 0x00, 0x1C, 0x00, 0x3C, 0x00, 0x1C, 0x00,
    //<key 6 >  < key 7   >
    0x1C, 0x00, 0x33, 0x00, 0x1C, 0x00, 0x1C, 0x00, 0x3C, 0x00, 0x3C, 0x00,
    0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00};

static const uint8_t g_signer_ca_priv_key[36] = {
    0x00, 0x00, 0x00, 0x00, 0x49, 0x0c, 0x1e, 0x09, 0xe2, 0x40, 0xaf, 0x00,
    0xe9, 0x6b, 0x43, 0x32, 0x92, 0x0e, 0x15, 0x8f, 0x69, 0x58, 0x8e, 0xd4,
    0x25, 0xc7, 0xf6, 0x8b, 0x0c, 0x6a, 0x52, 0x5d, 0x0d, 0x21, 0x4f, 0xee};

static const uint8_t g_access_key[32] = {
    0x32, 0x12, 0xd0, 0x66, 0xf5, 0xed, 0x52, 0xc7, 0x79, 0x98, 0xff,
    0xaa, 0xac, 0x43, 0x22, 0x60, 0xdd, 0xff, 0x9c, 0x10, 0x99, 0x6f,
    0x41, 0x66, 0x3a, 0x60, 0x23, 0xfa, 0xf6, 0xaa, 0x3e, 0xc5};

static void logD(char *str) {
  printf("atecc608A provisoning DEBUG: %s\n", str);
}

static int log_ret(int ret, char *str) {
  if (ret != ATCA_SUCCESS) {
    printf("atecc608A provisoning ERROR: %s ,code:%d\n", str, ret);
  }
  return ret;
}

int client_privision_self(void) {
  int ret = 0;
  bool lockstate = 0;
  uint8_t config_read_64[64];
  uint8_t pub_key_signer_ca[64];
  uint8_t pub_key_signer[64];
  uint8_t pub_key_device[64];

  logD("------client_privision_self begin");

  // 检查config_zone是否锁住
  ret = atcab_is_locked(LOCK_ZONE_CONFIG, &lockstate);
  if (log_ret(ret, "atcab_is_locked config_zone") != ATCA_SUCCESS)
    return ret;

  if (!lockstate) {
    // 设置config_zone
    ret = atcab_write_config_zone(g_ecc_configdata);
    if (log_ret(ret, "atcab_write_config_zone") != ATCA_SUCCESS)
      return ret;

    // 加锁config_zone
    ret = atcab_lock_config_zone();
    if (log_ret(ret, "atcab_lock_config_zone") != ATCA_SUCCESS)
      return ret;
  } else {
    printf("WARN: atecc608a config_zone has been locked\n");
  }

  // 写入配置和真实配置存在差异, 以读取配置为准, 读取两次,每次32byte
  ret = atcab_read_zone(ATCA_ZONE_CONFIG, 0, 0, 0, &config_read_64[0], 32);
  if (log_ret(ret, "atcab_read_zone config") != ATCA_SUCCESS)
    return ret;
  ret = atcab_read_zone(ATCA_ZONE_CONFIG, 0, 1, 0, &config_read_64[32], 32);
  if (log_ret(ret, "atcab_read_zone config") != ATCA_SUCCESS)
    return ret;

  // 检查data_zone 是否加锁
  ret = atcab_is_locked(LOCK_ZONE_DATA, &lockstate);
  if (log_ret(ret, "atcab_is_locked data_zone") != ATCA_SUCCESS)
    return ret;

  if (!lockstate) {
    // 设置ca_priv_key slot 7
    ret = atcab_priv_write(g_signer_ca_priv_key_slot, g_signer_ca_priv_key, 0,
                           NULL);
    if (log_ret(ret, "atcab_priv_write ca_priv_key_slot") != ATCA_SUCCESS)
      return ret;

    // 设置aceess_key slot 4
    ret = atcab_write_zone(ATCA_ZONE_DATA, g_access_key_slot, 0, 0,
                           g_access_key, 32);
    if (log_ret(ret, "atcab_write_zone access_key") != ATCA_SUCCESS)
      return ret;

    // 加锁data_zone
    ret = atcab_lock_data_zone();
    if (log_ret(ret, "atcab_lock_data_zone data_zone") != ATCA_SUCCESS)
      return ret;
  } else {
    printf("WARN: atecc608a data_zone has been locked\n");
  }

  printf("\nConfig in ATECC608A:\n");
  for (int i = 0; i < 64; i++) {
    printf("%2x ", config_read_64[i]);
    if ((i + 1) % 12 == 0)
      printf("\n");
  }
  printf("\n\n");

  // 生成signer_ca_pub_key slot 7
  ret = atcab_get_pubkey(g_signer_ca_priv_key_slot, pub_key_signer_ca);
  if (log_ret(ret, "atcab_get_pubkey singner_ca") != ATCA_SUCCESS)
    return ret;
  atcab_printbin_label("Signer CA Public Key:\r\n", pub_key_signer_ca,
                       ATCA_PUB_KEY_SIZE);

  // 生成signer_pub_key slot 2
  ret = atcab_genkey(g_signer_priv_key_slot, pub_key_signer);
  if (log_ret(ret, "atcab_genkey signer_key") != ATCA_SUCCESS)
    return ret;
  atcab_printbin_label("Signer Public Key:\r\n", pub_key_signer,
                       ATCA_PUB_KEY_SIZE);

  // 生成device_pub_key slot 0
  ret = atcab_genkey(g_device_priv_key_slot, pub_key_device);
  if (log_ret(ret, "atcab_genkey device_key") != ATCA_SUCCESS)
    return ret;
  atcab_printbin_label("Device Public Key:\r\n", pub_key_device,
                       ATCA_PUB_KEY_SIZE);

  logD("------client_privision_self end");

  return 0;
}
