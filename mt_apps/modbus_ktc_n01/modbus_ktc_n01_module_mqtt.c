#include "stdio.h"
#include "string.h"

#include "esp_err.h"
#include "esp_log.h"
#include "mqtt_client.h"

#include "google/protobuf/any.pb-c.h"
#include "google/protobuf/empty.pb-c.h"
#include "stream_frame.pb-c.h"

#include "mt_module_flow.h"
#include "mt_module_flow_manage.h"
#include "mt_module_mqtt.h"
#include "mt_module_unarycall_utils.h"
#include "mt_mqtt_lan.h"
#include "mt_mqtt_utils.h"
#include "mt_proto_ktc_n01.pb-c.h"
#include "mt_utils_error.h"

#include "modbus_ktc_n01.h"
#include "modbus_ktc_n01_utils.h"

// global config ==============================================================

static const char *TAG = "MODULE_MQTT";

// static func ================================================================

static void
module_mqtt_process_set_cmd(Ai__Metathings__Component__DownStreamFrame *msg,
                            char *module_id) {
  esp_err_t err = ESP_OK;
  MtKtcN01__SetCmdReq *req = NULL;
  int res_size = 0;
  uint8_t *res_buf = NULL;
  int frame_size = 0;
  uint8_t *frame_buf = NULL;
  MtKtcN01__SetCmdRes res = MT_KTC_N01__SET_CMD_RES__INIT;
  char topic[256] = "";

  err = mt_module_unarycall_utils_check(msg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mt_module_unarycall_utils_check failed", __LINE__,
             __func__);
    res.code = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // parse req
  req = mt_ktc_n01__set_cmd_req__unpack(NULL, msg->unary_call->value->value.len,
                                        msg->unary_call->value->value.data);
  err = modbus_ktc_n01_util_check_set_cmd_req(req);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_ktc_n01_util_check_set_cmd_req failed",
             __LINE__, __func__);
    res.code = MT_ERR_INVALID_ARG;
    goto ERROR;
  }

  // cmd process
  err = modbus_ktc_n01_set_cmd(req->port, req->cmd);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s modbus_ktc_n01_set_cmd failed", __LINE__, __func__);
    res.code = MT_ERR_INVALID_RESPONSE;
    goto ERROR;
  }

ERROR:
  res_size = mt_ktc_n01__set_cmd_res__get_packed_size(&res);
  res_buf = malloc(res_size);
  mt_ktc_n01__set_cmd_res__pack(&res, res_buf);

  // marshal data
  frame_buf = mt_module_unarycall_utils_pack(
      res_buf, res_size, mt_ktc_n01__set_cmd_res__descriptor.name, msg,
      &frame_size);

  // response
  sprintf(topic, "mt/modules/%s/proxy/sessions/%lld/upstream", module_id,
          msg->unary_call->session->value);
  err = mqtt_pub_msg(topic, frame_buf, frame_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "%4d %s mqtt_pub_msg failed", __LINE__, __func__);
    goto EXIT;
  }

  if (res.code == 0) {
    ESP_LOGI(TAG, "%4d %s session:%lld port:%d set cmd:%d response", __LINE__,
             __func__, msg->unary_call->session->value, req->port, req->cmd);
  } else {
    ESP_LOGE(TAG, "%4d %s failed", __LINE__, __func__);
  }

EXIT:
  if (req != NULL)
    mt_ktc_n01__set_cmd_req__free_unpacked(req, NULL);

  free(res_buf);
  free(frame_buf);
  return;
}

// global func  ===============================================================

void modbus_ktc_n01_module_mqtt_init() {
  mt_module_mqtt_add_handle(module_mqtt_process_set_cmd, "SetCmd");
}
