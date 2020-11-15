
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "rs232_charge001.h"
#include "rs232_charge001_lora_unarycall.h"
#include "rs232_lora_ebyte.h"
#include "rs232_lora_ebyte_module_manage.h"

#include "mt_module_unarycall_utils.h"
#include "mt_proto_charge001.pb-c.h"
#include "stream_frame.pb-c.h"

// static define ==============================================================

static const char *TAG = "RS232_CHARGE001_LORA_UNARYCALL";

// static func ================================================================

static void rs232_charge001_lora_unarycall_loop() {
  esp_err_t err = ESP_OK;
  rs232_lora_ebyte_data_t *ebyte_recv = NULL;
  rs232_lora_ebyte_data_t *ebyte_data = NULL;
  Ai__Metathings__Component__DownStreamFrame *msg = NULL;

  while (true) {
    ebyte_recv = rs232_lora_ebyte_recv();
    if (ebyte_recv == NULL) {
      // ESP_LOGW(TAG, "%4d %s rs232_lora_ebyte_recv NULL", __LINE__, __func__);
    } else {
      ebyte_data = rs232_lora_ebyte_new_data();
      if (2 == ebyte_recv->id) {
        if (ebyte_recv->cmd == RS232_LORA_EBYTE_CMD_TYPE_UNARYCALL) {
          MtCharge001__SetChargeReq *set_req = NULL;
          ebyte_data->id = ebyte_recv->id;
          ebyte_data->type = 2;
          ebyte_data->cmd = ebyte_recv->cmd;
          ebyte_data->handle = ebyte_recv->handle;

          // parse msg
          msg = ai__metathings__component__down_stream_frame__unpack(
              NULL, ebyte_recv->len, ebyte_recv->data);
          if (msg == NULL) {
            ESP_LOGE(TAG, "%4d %s get null msg or unknown msg", __LINE__,
                     __func__);
            goto EXIT;
          }

          // parse data
          set_req = mt_charge001__set_charge_req__unpack(
              NULL, msg->unary_call->value->value.len,
              msg->unary_call->value->value.data);
          if (set_req == NULL) {
            ESP_LOGE(TAG, "%4d %s mt_charge001__set_charge_req__unpack NULL",
                     __LINE__, __func__);
            continue;
          }

          // process cmd
          int32_t result = 0;
          int32_t res_port = 0;
          err = rs232_charge001_set_charge(
              set_req->charge->port, set_req->charge->money,
              set_req->charge->time, &res_port, &result);
          if (err != ESP_OK) {
            ESP_LOGE(TAG, "%4d %s rs232_charge001_set_charge failed", __LINE__,
                     __func__);
          }

          // response
          int res_size = 0;
          uint8_t *res_buf = NULL;
          MtCharge001__SetRes res = MT_CHARGE001__SET_RES__INIT;
          res.code = err;
          res_size = mt_charge001__set_res__get_packed_size(&res);
          res_buf = malloc(res_size);
          mt_charge001__set_res__pack(&res, res_buf);
          ebyte_data->data = mt_module_unarycall_utils_pack(
              res_buf, res_size, mt_charge001__set_res__descriptor.name, msg,
              (int *)&ebyte_data->len);
          if (res_buf != NULL)
            free(res_buf);

          err = rs232_lora_ebyte_sent(ebyte_data);
          if (err != ESP_OK) {
            ESP_LOGE(TAG, "%4d %s rs232_lora_ebyte_sent failed", __LINE__,
                     __func__);
          } else {
            ESP_LOGI(TAG, "%4d %s sent response", __LINE__, __func__);
          }
        } else {
          ESP_LOGW(TAG, "%4d %s unexcept cmd:%d", __LINE__, __func__,
                   ebyte_recv->cmd);
        }
      } else {
        ESP_LOGW(TAG, "%4d %s unexcept id:%d", __LINE__, __func__,
                 ebyte_recv->id);
      }

    EXIT:
      ai__metathings__component__down_stream_frame__free_unpacked(msg, NULL);
      msg = NULL;
      rs232_lora_ebyte_free_data(ebyte_data);
      ebyte_data = NULL;
      rs232_lora_ebyte_free_data(ebyte_recv);
      ebyte_recv = NULL;
    }

    // vTaskDelay(10 / portTICK_RATE_MS);
  }

  vTaskDelete(NULL);
}

// global func ================================================================

esp_err_t rs232_charge001_lora_unarycall_task() {
  esp_err_t err = ESP_OK;

  xTaskCreate((TaskFunction_t)rs232_charge001_lora_unarycall_loop,
              "UNARYCALL_TASK", 8 * 1024, NULL, 10, NULL);

  return err;
}
