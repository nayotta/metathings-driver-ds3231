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

#include "esp_eth.h"
#define DEFAULT_ETHERNET_PHY_CONFIG phy_lan8720_default_ethernet_config

// static define ==============================================================
static const char *TAG = "mt_ethernet";

static mt_gpio_light_t *LIGHT_HANDLE = NULL;

// static func ===============================================================

static void eth_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data) {
  uint8_t mac_addr[6] = {0};
  /* we can get the ethernet driver handle from event data */
  esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
  bool ret = false;

  switch (event_id) {
  case ETHERNET_EVENT_CONNECTED:
    esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
    ESP_LOGI(TAG, "Ethernet Link Up");
    ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0],
             mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    break;
  case ETHERNET_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "Ethernet Link Down");
    if (LIGHT_HANDLE != NULL) {
      ret = mt_gpio_light_set_blink(LIGHT_HANDLE, 2000);
      if (ret == false) {
        ESP_LOGE(TAG, "%d mt_gpio_light_set_blink failed", __LINE__);
      }
    }
    break;
  case ETHERNET_EVENT_START:
    ESP_LOGI(TAG, "Ethernet Started");
    break;
  case ETHERNET_EVENT_STOP:
    ESP_LOGI(TAG, "Ethernet Stopped");
    break;
  default:
    break;
  }
}

static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data) {
  ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
  const tcpip_adapter_ip_info_t *ip_info = &event->ip_info;
  bool ret = false;

  ESP_LOGI(TAG, "Ethernet Got IP Address");
  ESP_LOGI(TAG, "~~~~~~~~~~~");
  ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
  ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
  ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
  ESP_LOGI(TAG, "~~~~~~~~~~~");

  if (LIGHT_HANDLE != NULL) {
    ret = mt_gpio_light_set_on(LIGHT_HANDLE);
    if (ret == false) {
      ESP_LOGE(TAG, "%d mt_gpio_light_set_on failed", __LINE__);
    }
  }
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

  ESP_ERROR_CHECK(esp_event_loop_create_default());
  ESP_ERROR_CHECK(tcpip_adapter_set_default_eth_handlers());
  ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID,
                                             &eth_event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP,
                                             &got_ip_event_handler, NULL));

  eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
  eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();

  esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);

  esp_eth_phy_t *phy = esp_eth_phy_new_lan8720(&phy_config);

  esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
  esp_eth_handle_t eth_handle = NULL;
  ESP_ERROR_CHECK(esp_eth_driver_install(&config, &eth_handle));
}
