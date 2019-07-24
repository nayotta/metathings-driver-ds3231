#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"

#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "mt_mqtt.h"
#include "mt_mqtt_lan.h"
#include "nvs_flash.h"

// global define ==============================================================
#define MQTT_USE_LAN
#ifdef MQTT_USE_LAN

#endif

#ifdef MQTT_USE_AIR720H
#include "mt_mqtt_air720h.h"
#endif

// global value ===============================================================
static const char *TAG = "MT_MQTT";

long unsigned int App_Start_Time = 0; // task start time
int Restart_Times = -1;               // task start count

// static value ===============================================================
static int Keep_Count = 0;                      // keepalive count
static int Keep_Count_Limit = 3;                // keepalive limit
static int keepalive_Interval = 30;             // keepalive count 90s
static long unsigned int LastKeepAliveTime = 0; // keepalive record

// static func ===============================================================
static TaskFunction_t mt_mqtt_loop(void *pvParameters)
{
RESET:
  ESP_LOGI(TAG, "%d %s start", __LINE__, __func__);

  // count reset
  Keep_Count = 0;
  App_Start_Time = (xTaskGetTickCount() * portTICK_RATE_MS) / 1000;
  Restart_Times++;

  // heartbeat loop
  while (1)
  {
    if (xTaskGetTickCount() - LastKeepAliveTime >=
        keepalive_Interval * 1000 / portTICK_RATE_MS)

    {
      // send heartbeat
      ESP_LOGI(TAG, "%d heartbeat request", __LINE__);

      // mt_mqtt_pub_heartbeat();

      // check heartbeat failed
      if (Keep_Count >= Keep_Count_Limit)
      {
        ESP_LOGE(TAG, "%d Heartbeat failed reach limited, restart app",
                 __LINE__);
        goto RESET;
      }

      Keep_Count++;
      LastKeepAliveTime = xTaskGetTickCount();
    }

    vTaskDelay(500 / portTICK_RATE_MS);
  }

  return NULL;
}

// global func ================================================================
int mt_mqtt_pub_msg(char *topic, uint8_t *buf, int size)
{
  int ret = 0;

  ret = mqtt_pub_msg(topic, buf, size);

  return ret;
}

int mt_mqtt_init(char *host, char *port, char *username, char *password,
                 char *module_id, uint64_t session_id, char *device_id,
                 void (*handle)(char *topic, void *buf, int size))
{
  esp_err_t err;

  err = mqtt_init(host, port, username, password, module_id, session_id,
                  device_id, handle);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%d  mqtt_init failed", __LINE__);
  }

  return err;
}

void mt_mqtt_task()
{
  ESP_LOGI(TAG, "Mqtt init");

  // task start
  ESP_LOGI(TAG, "Mqtt task start");
  xTaskCreate((TaskFunction_t)mt_mqtt_loop, "mt_mqtt_task", 8192, NULL, 2,
              NULL);
}
