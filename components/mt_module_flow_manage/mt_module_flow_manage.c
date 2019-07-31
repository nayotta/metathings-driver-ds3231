#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "mt_module_flow.h"
#include "mt_module_flow_manage.h"
#include "mt_mqtt_utils.h"

// global define ==============================================================
static const char *TAG = "MT_MODULE_FLOW_MANAGE";

// global func ================================================================
esp_err_t mt_module_flow_manage_mqtt_process(char *topic, uint8_t *buf,
                                             uint8_t size)
{
    char *session_string = NULL;
    bool match = false;

    // check MT_MODULE_FLOW_MANAGE
    if (MT_MODULE_FLOW_MANAGE == NULL)
    {
        ESP_LOGE(TAG, "%4d %s MT_MODULE_FLOW_MANAGE is NULL", __LINE__, __func__);
        return ESP_ERR_INVALID_ARG;
    }
    else
    {
        if (MT_MODULE_FLOW_MANAGE->flows_size == 0)
        {
            ESP_LOGE(TAG, "%4d %s MT_MODULE_FLOW_MANAGE->flows_size is zero",
                     __LINE__, __func__);
            return ESP_ERR_INVALID_ARG;
        }

        if (MT_MODULE_FLOW_MANAGE->flows == NULL)
        {
            ESP_LOGE(TAG, "%4d %s MT_MODULE_FLOW_MANAGE->flows is NULL", __LINE__,
                     __func__);
            return ESP_ERR_INVALID_ARG;
        }
    }

    // get session_id
    session_string = mt_mqtt_utils_get_session_string_from_topic(topic);
    if (session_string == NULL)
    {
        ESP_LOGE(TAG,
                 "%4d %s mt_mqtt_utils_get_session_id_from_topic failed, topic:%s",
                 __LINE__, __func__, topic);
        return ESP_ERR_INVALID_ARG;
    }

    //
    for (int i = 0; i < MT_MODULE_FLOW_MANAGE->flows_size; i++)
    {
        match = true;
        if (MT_MODULE_FLOW_MANAGE->flows[i] == NULL)
        {
            ESP_LOGE(TAG, "%4d %s MT_MODULE_FLOW_MANAGE->flows[%d] is NULL", __LINE__,
                     __func__, i);
            return ESP_ERR_INVALID_ARG;
        }

        if (strcmp(MT_MODULE_FLOW_MANAGE->flows[i]->session, session_string) == 0)
        {
            mt_module_flow_process(MT_MODULE_FLOW_MANAGE->flows[i], topic, buf, size);
        }
    }

    if (match != true)
    {
        ESP_LOGE(TAG, "%4d %s unexcept session_id:%s", __LINE__, __func__,
                 session_string);
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}