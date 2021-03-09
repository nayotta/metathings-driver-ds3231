#include "stdio.h"
#include "string.h"

#include "esp_err.h"
#include "esp_log.h"
#include "mqtt_client.h"

#include "google/protobuf/any.pb-c.h"
#include "google/protobuf/empty.pb-c.h"

#include "stream_frame.pb-c.h"

#include "gpio_maglock001.h"
#include "mt_module_flow_manage.h"
#include "mt_module_mqtt.h"
#include "mt_module_unarycall_utils.h"
#include "mt_mqtt_lan.h"
#include "mt_mqtt_utils.h"

// global config ==============================================================

static const char *TAG = "MODULE_MQTT";

// static func ================================================================

static void
module_mqtt_process_get_state(Ai__Metathings__Component__DownStreamFrame *msg,
                              char *module_id) {
  esp_err_t err = ESP_OK;
  int res_size = 0;
  int module_index = 1;
  uint8_t *res_buf = NULL;
  int port_num = 0;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;

  // cmd process
  err = gpio_maglock001_get_port_num(&port_num);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s gpio_maglock001_get_port_num index:%d failed",
             __LINE__, __func__, module_index);
    return;
  }

  bool *state = (bool *)malloc(port_num * sizeof(bool));

  for (int i = 0; i < port_num; i++) {
    err = gpio_maglock001_get_state(i + 1, &state[i]);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "%4d %s mt_maglock_get_state index:%d failed", __LINE__,
               __func__, i + 1);
      return;
    }
  }

  // res body
  MtMaglock__GetStateRes res = MT_MAGLOCK__GET_STATE_RES__INIT;
  res.n_state = port_num;
  res.state =
      (MtMaglock__State **)malloc(port_num * sizeof(MtMaglock__State *));
  for (int i = 0; i < port_num; i++) {
    res.state[i] = (MtMaglock__State *)malloc(sizeof(MtMaglock__State));
    mt_maglock__state__init(res.state[i]);
    res.state[i]->addr = i + 1;
    res.state[i]->state = state[i];
  }

  res_size = mt_maglock__get_state_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_maglock__get_state_res__pack(&res, res_buf);

  // marshal data
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt_maglock__get_state_res__descriptor.name, msg,
      &frame_size);

  // response
  char topic[256] = "";
  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  ESP_LOGI(TAG, "%4d %s session:%lld response", __LINE__, __func__,
           msg->unary_call->session->value);
  for (int i = 0; i < port_num; i++) {
    ESP_LOGI(TAG, "%4d %s add:%d state:%d", __LINE__, __func__, i + 1,
             state[i]);
  }

EXIT:
  free(res_buf);
  free(frame_buf);
  return;
}

// global func  ===============================================================
void gpio_moglock001_module_mqtt_init() {
  mt_module_mqtt_add_handle(module_mqtt_process_get_state, "GetState");
}
