#include "freertos/FreeRTOS.h"

#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "esp_err.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/task.h"
#include "rom/gpio.h"
#include "tcpip_adapter.h"
#include <stdio.h>
#include <string.h>

#include "gpio_light.h"

#include "eth_phy/phy_lan8720.h"
#define DEFAULT_ETHERNET_PHY_CONFIG phy_lan8720_default_ethernet_config

// static define ==============================================================
static const char *TAG = "eth_example";

static mt_gpio_light_t *LIGHT_HANDLE = NULL;

// static func ===============================================================
/**
 * @brief gpio specific init
 *
 * @note RMII data pins are fixed in esp32:
 * TXD0 <=> GPIO19
 * TXD1 <=> GPIO22
 * TX_EN <=> GPIO21
 * RXD0 <=> GPIO25
 * RXD1 <=> GPIO26
 * CLK <=> GPIO0
 *
 */
static void eth_gpio_config_rmii(void) {
  phy_rmii_configure_data_interface_pins();
  phy_rmii_smi_configure_pins(23, 18);
}

/**
 * @brief event handler for ethernet
 *
 * @param ctx
 * @param event
 * @return esp_err_t
 */
static esp_err_t eth_event_handler(void *ctx, system_event_t *event) {
  bool ret = false;
  tcpip_adapter_ip_info_t ip;

  switch (event->event_id) {
  case SYSTEM_EVENT_ETH_CONNECTED:
    ESP_LOGI(TAG, "Ethernet Link Up");
    break;
  case SYSTEM_EVENT_ETH_DISCONNECTED:
    ESP_LOGI(TAG, "Ethernet Link Down");
    if (LIGHT_HANDLE != NULL) {
      ret = mt_gpio_light_set_blink(LIGHT_HANDLE, 2000);
      if (ret == false) {
        ESP_LOGE(TAG, "%d mt_gpio_light_set_blink failed", __LINE__);
      }
    }
    break;
  case SYSTEM_EVENT_ETH_START:
    ESP_LOGI(TAG, "Ethernet Started");
    break;
  case SYSTEM_EVENT_ETH_GOT_IP:
    memset(&ip, 0, sizeof(tcpip_adapter_ip_info_t));
    ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(ESP_IF_ETH, &ip));
    ESP_LOGI(TAG, "Ethernet Got IP Addr");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip.ip));
    ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip.netmask));
    ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip.gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    if (LIGHT_HANDLE != NULL) {
      ret = mt_gpio_light_set_on(LIGHT_HANDLE);
      if (ret == false) {
        ESP_LOGE(TAG, "%d mt_gpio_light_set_on failed", __LINE__);
      }
    }
    break;
  case SYSTEM_EVENT_ETH_STOP:
    ESP_LOGI(TAG, "Ethernet Stopped");
    break;
  default:
    break;
  }
  return ESP_OK;
}

// global func ================================================================

void mt_ethernet_task(int light_pin, int light_pin_on_level) {
  esp_err_t err = ESP_OK;

  if (light_pin != -1) {
    LIGHT_HANDLE = mt_gpio_light_new(light_pin, light_pin_on_level);
    err = mt_gpio_light_task(LIGHT_HANDLE);
    if (err == false) {
      ESP_LOGE(TAG, "%d mt_gpio_light_task %d create failed", __LINE__,
               LIGHT_HANDLE->pin);
    }
  }

  tcpip_adapter_init();

  ESP_ERROR_CHECK(esp_event_loop_init(eth_event_handler, NULL));

  eth_config_t config = DEFAULT_ETHERNET_PHY_CONFIG;
  config.phy_addr = 0;
  config.gpio_config = eth_gpio_config_rmii;
  config.tcpip_input = tcpip_adapter_eth_input;
  config.clock_mode = 1;

  ESP_ERROR_CHECK(esp_eth_init(&config));
  ESP_ERROR_CHECK(esp_eth_enable());
}
