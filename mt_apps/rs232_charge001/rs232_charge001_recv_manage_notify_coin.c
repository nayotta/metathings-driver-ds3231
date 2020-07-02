#include "rs232_charge001_recv_manage_notify_coin.h"

#include "mt_module_flow_manage.h"
#include "mt_utils_string.h"

// static define ==============================================================

static const char *TAG = "rs232_charge001_recv_manage_notify_coin";
static int32_t MIN_RES_SIZE = 3;

// global func ================================================================

void rs232_charge001_recv_manage_notify_coin_push_flow(uint8_t *buf,
                                                       uint32_t size) {
  esp_err_t err = ESP_OK;
  mt_module_flow_t *module_flow = NULL;
  mt_module_flow_struct_group_t *data = NULL;
  int count = 0;

  if (size < MIN_RES_SIZE) {
    ESP_LOGE(TAG, "%4d %s size too small:%d", __LINE__, __func__, size);
    return;
  }

  data = mt_module_flow_new_struct_group(2);

  // notify
  data->value[count]->key = mt_utils_string_copy("notify");
  data->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_BOOL_VALUE;
  data->value[count++]->bool_value = true;

  // coin
  data->value[count]->key = mt_utils_string_copy("coin");
  data->value[count]->type = GOOGLE__PROTOBUF__VALUE__KIND_NUMBER_VALUE;
  if (size == 1)
    data->value[count++]->number_value = 1.0;
  else
    data->value[count++]->number_value = buf[0] * 1.0;

  module_flow = mt_module_flow_manage_get_flow_by_flow_index(1);
  if (module_flow == NULL) {
    ESP_LOGE(TAG, "%4d %s could not find flow in manage", __LINE__, __func__);
    goto EXIT;
  }

  err = mt_module_flow_sent_msg(module_flow, data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_flow_sent_msg failed", __LINE__, __func__);
    goto EXIT;
  }

EXIT:
  mt_module_flow_free_struct_group(data);
}
