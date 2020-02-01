#include "cryptoauthlib.h"
#include "driver/i2c.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

#include "nyt_hmac.h"
#include "provision_self.h"

void app_main() {
  ATCAIfaceCfg cfg = {.iface_type = ATCA_I2C_IFACE,
                      .devtype = ATECC608A,
                      .atcai2c.slave_address = 0XC0,
                      .atcai2c.bus = 0,
                      .atcai2c.baud = 100000,
                      .wake_delay = 1500,
                      .rx_retries = 20};

  uint8_t rand_out[32] = {0};
  int ret;

  ret = atcab_init(&cfg);
  printf("atcab_init ret:%d\n", ret);
  if (ret != 0) {
    return;
  }

  ret = atcab_read_serial_number(rand_out);

  printf("atcab_random ret:%d\n", ret);
  printf("------>\n");
  for (int i = 0; i < 9; i++) {
    printf("%x ", rand_out[i]);
  }
  printf("\n<------\n");

  ret = client_privision_self();

  printf("\n hmac test\n");
  nyt_hmac();
  printf("\n hmac test end\n");
}
