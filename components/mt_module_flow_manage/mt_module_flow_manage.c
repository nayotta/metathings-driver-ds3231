#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "mt_module_flow.h"
#include "mt_module_flow_manage.h"
#include "mt_mqtt_utils.h"

// global define ==============================================================
static const char *TAG = "MT_MODULE_FLOW_MANAGE";
static mt_module_flow_manage_t *FLOW_MANAGE = NULL;

// global func ================================================================
esp_err_t mt_module_flow_manage_mqtt_process(char *topic, uint8_t *buf,
                                             uint8_t size)
{
  esp_err_t err = ESP_OK;
  char *session_string = NULL;
  bool match = false;

  // check FLOW_MANAGER
  if (FLOW_MANAGE == NULL)
  {
    ESP_LOGE(TAG, "%4d %s FLOW_MANAGER is NULL", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }
  else
  {
    if (FLOW_MANAGE->flows_size == 0)
    {
      ESP_LOGE(TAG, "%4d %s FLOW_MANAGER->flows_size is zero", __LINE__,
               __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }

    if (FLOW_MANAGE->flows == NULL)
    {
      ESP_LOGE(TAG, "%4d %s FLOW_MANAGER->flows is NULL", __LINE__, __func__);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }

  // get session_id
  session_string = mt_mqtt_utils_get_session_string_from_topic(topic);
  if (session_string == NULL)
  {
    ESP_LOGE(TAG,
             "%4d %s mt_mqtt_utils_get_session_id_from_topic failed, topic:%s",
             __LINE__, __func__, topic);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  // match session
  for (int i = 0; i < FLOW_MANAGE->flows_size; i++)
  {
    if (FLOW_MANAGE->flows[i] == NULL)
    {
      ESP_LOGE(TAG, "%4d %s FLOW_MANAGER->flows[%d] is NULL", __LINE__,
               __func__, i);
      continue;
    }

    if (FLOW_MANAGE->flows[i]->session == NULL)
    {
      ESP_LOGE(TAG, "%4d %s FLOW_MANAGER->flows[%d]->session is NULL", __LINE__,
               __func__, i);
      continue;
    }

    if (strcmp(FLOW_MANAGE->flows[i]->session, session_string) == 0)
    {
      match = true;
      mt_module_flow_process(FLOW_MANAGE->flows[i], topic, buf, size);
    }
  }

  if (match != true)
  {
    ESP_LOGE(TAG, "%4d %s unexcept session_id:%s", __LINE__, __func__,
             session_string);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

EXIT:
  if (session_string != NULL)
  {
    free(session_string);
  }
  return err;
}

esp_err_t mt_module_flow_manage_add(mt_module_flow_t *flow)
{
  mt_module_flow_manage_t *temp_manage = NULL;

  if (FLOW_MANAGE == NULL)
  {
    FLOW_MANAGE = malloc(sizeof(mt_module_flow_t));
    FLOW_MANAGE->flows_size = 1;
    FLOW_MANAGE->flows = malloc(1 * sizeof(mt_module_flow_t));
    FLOW_MANAGE->flows[0] = flow;
    return ESP_OK;
  }

  temp_manage = FLOW_MANAGE;
  FLOW_MANAGE =
      malloc((temp_manage->flows_size + 1) * sizeof(mt_module_flow_t));
  FLOW_MANAGE->flows_size = temp_manage->flows_size + 1;
  FLOW_MANAGE->flows =
      malloc(FLOW_MANAGE->flows_size * sizeof(mt_module_flow_t));
  for (int i = 0; i < FLOW_MANAGE->flows_size - 1; i++)
  {
    FLOW_MANAGE->flows[i] = temp_manage->flows[i];
  }
  FLOW_MANAGE->flows[FLOW_MANAGE->flows_size - 1] = flow;

  free(temp_manage);
  return ESP_OK;
}
