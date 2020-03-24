#include "esp_log.h"

#include "rs232_lora_ebyte.h"
#include "rs232_lora_ebyte_flow_manage.h"

#include "mt_mqtt_lan.h"

// static define ==============================================================

static const char *TAG = "RS232_LORA_EBYTE_FLOW_MANAGE";
static rs232_lora_ebyte_flow_manage_t *FLOW_MANAGE = NULL;

// static define ==============================================================

esp_err_t
rs232_lora_ebyte_flow_manage_mqtt_process(rs232_lora_ebyte_data_t *ebyte_data) {
  esp_err_t err = ESP_OK;

  // check FLOW_MANAGE
  if (FLOW_MANAGE == NULL) {
    ESP_LOGE(TAG, "%4d %s FLOW_MANAGE NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // match addr
  int match = 0;
  for (int i = 0; i < FLOW_MANAGE->flows_size; i++) {
    if (FLOW_MANAGE->flows_addr[i] == ebyte_data->id) {
      match++;
      char topic[256] = "";
      if (FLOW_MANAGE->flows[i]->session == NULL) {
        ESP_LOGE(TAG, "%4d %s FLOW_MANAGE->flows[i]->session  NULL", __LINE__,
                 __func__);
        continue;
      }
      if (FLOW_MANAGE->flows[i]->module_http->module->deviceID == NULL) {
        ESP_LOGE(
            TAG,
            "%4d %s FLOW_MANAGE->flows[i]->module_http->module->deviceID  NULL",
            __LINE__, __func__);
        continue;
      }

      sprintf(topic, "mt/devices/%s/flow_channel/sessions/%s/upstream",
              FLOW_MANAGE->flows[i]->module_http->module->deviceID,
              FLOW_MANAGE->flows[i]->session);
      mqtt_pub_msg(topic, ebyte_data->data, ebyte_data->len);
    }
  }

  if (match == 0) {
    ESP_LOGE(TAG, "%4d %s unmatch lora addr msg:%d", __LINE__, __func__,
             ebyte_data->id);
  }

EXIT:
  if (ebyte_data != NULL) {
    rs232_lora_ebyte_free_data(ebyte_data);
  }
  return err;
}

// global define ==============================================================

esp_err_t rs232_lora_ebyte_flow_manage_add(mt_module_flow_t *flow,
                                           int flow_addr,
                                           QueueHandle_t flow_handle) {
  esp_err_t err = ESP_OK;

  rs232_lora_ebyte_flow_manage_t *temp_manage = NULL;

  if (FLOW_MANAGE == NULL) {
    FLOW_MANAGE = malloc(sizeof(rs232_lora_ebyte_flow_manage_t));
    FLOW_MANAGE->flows_size = 1;
    FLOW_MANAGE->flows = malloc(1 * sizeof(mt_module_flow_t *));
    FLOW_MANAGE->flows[0] = flow;
    FLOW_MANAGE->flows_addr = malloc(1 * sizeof(int));
    FLOW_MANAGE->flows_addr[0] = flow_addr;
    FLOW_MANAGE->flows_handle = malloc(1 * sizeof(QueueHandle_t *));
    FLOW_MANAGE->flows_handle[0] = flow_handle;
    return err;
  }

  temp_manage = FLOW_MANAGE;
  FLOW_MANAGE = malloc(sizeof(rs232_lora_ebyte_flow_manage_t));
  FLOW_MANAGE->flows_size = temp_manage->flows_size + 1;
  FLOW_MANAGE->flows =
      malloc(FLOW_MANAGE->flows_size * sizeof(mt_module_flow_t *));
  for (int i = 0; i < FLOW_MANAGE->flows_size - 1; i++) {
    FLOW_MANAGE->flows[i] = temp_manage->flows[i];
  }
  FLOW_MANAGE->flows_addr = malloc(FLOW_MANAGE->flows_size * sizeof(int));
  for (int i = 0; i < FLOW_MANAGE->flows_size - 1; i++) {
    FLOW_MANAGE->flows_addr[i] = temp_manage->flows_addr[i];
  }
  for (int i = 0; i < FLOW_MANAGE->flows_size - 1; i++) {
    FLOW_MANAGE->flows_handle[i] = temp_manage->flows_handle[i];
  }
  FLOW_MANAGE->flows[FLOW_MANAGE->flows_size - 1] = flow;
  FLOW_MANAGE->flows_addr[FLOW_MANAGE->flows_size - 1] = flow_addr;
  FLOW_MANAGE->flows_handle[FLOW_MANAGE->flows_size - 1] = flow_handle;

  free(temp_manage);
  return err;
}

rs232_lora_ebyte_flow_manage_t *rs232_lora_ebyte_flow_manage_get() {
  return FLOW_MANAGE;
}
