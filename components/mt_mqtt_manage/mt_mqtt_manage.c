#include "mt_mqtt_manage.h"

// global config ==============================================================
static const char *TAG = "MT_MQTT_MANAGE";
static mt_mqtt_manage_t *MANAGE = NULL;

// global func ================================================================
esp_err_t mt_mqtt_manage_add(mt_mqtt_manage_client_t *client_in)
{
  mt_mqtt_manage_t *temp_manage = NULL;

  if (MANAGE == NULL)
  {
    MANAGE = malloc(sizeof(mt_mqtt_manage_t));
    MANAGE->mqtt_num = 1;
    MANAGE->clients =
        malloc(1 * sizeof(mt_mqtt_manage_client_t));
    MANAGE->clients[0] = client_in;
    return ESP_OK;
  }

  temp_manage = MANAGE;
  MANAGE = malloc((temp_manage->mqtt_num + 1) * sizeof(mt_mqtt_manage_t));
  MANAGE->mqtt_num = temp_manage->mqtt_num + 1;
  MANAGE->clients = malloc(MANAGE->mqtt_num * sizeof(mt_mqtt_manage_t));
  for (int i = 0; i < MANAGE->mqtt_num - 1; i++)
  {
    MANAGE->clients[i] = temp_manage->clients[i];
  }
  MANAGE->clients[MANAGE->mqtt_num - 1] = client_in;

  free(temp_manage);
  return ESP_OK;
}

char *mt_mqtt_manage_get_module_id(esp_mqtt_client_handle_t *client)
{
  char *module_id = NULL;
  int module_id_size = 0;

  if (MANAGE == NULL)
    return NULL;

  for (int i = 0; i < MANAGE->mqtt_num; i++)
  {
    if (MANAGE->clients[i]->client == client)
    {
      module_id_size = strlen(MANAGE->clients[i]->module_id);
      module_id = malloc(module_id_size + 1);
      memcpy(module_id, MANAGE->clients[i]->module_id, module_id_size);
      module_id[module_id_size] = '\0';
      return module_id;
    }
  }

  return NULL;
}

esp_err_t mt_mqtt_manage_set_module_id(esp_mqtt_client_handle_t *client_in,
                                       char *module_id_in)
{
  if (MANAGE == NULL)
    return ESP_ERR_INVALID_ARG;

  for (int i = 0; i < MANAGE->mqtt_num; i++)
  {
    if (client_in == MANAGE->clients[i]->client)
    {
      if (MANAGE->clients[i]->module_id != NULL)
        free(MANAGE->clients[i]->module_id);
      int module_id_size = strlen(module_id_in) + 1;
      MANAGE->clients[i]->module_id = malloc(module_id_size);
      memcpy(MANAGE->clients[i]->module_id, module_id_in, module_id_size - 1);
      MANAGE->clients[i]->module_id[module_id_size] = '\0';
      return ESP_OK;
    }
  }

  return ESP_ERR_INVALID_ARG;
}

char *mt_mqtt_manage_get_device_id(esp_mqtt_client_handle_t *client)
{
  char *dev_id = NULL;
  int dev_id_size = 0;

  if (MANAGE == NULL)
    return NULL;

  for (int i = 0; i < MANAGE->mqtt_num; i++)
  {
    if (MANAGE->clients[i]->client == client)
    {
      dev_id_size = strlen(MANAGE->clients[i]->device_id);
      dev_id = malloc(dev_id_size + 1);
      memcpy(dev_id, MANAGE->clients[i]->device_id, dev_id_size);
      dev_id[dev_id_size] = '\0';
      return dev_id;
    }
  }

  return NULL;
}

esp_err_t mt_mqtt_manage_set_device_id(esp_mqtt_client_handle_t *client_in,
                                       char *dev_id_in)
{
  if (MANAGE == NULL)
    return ESP_ERR_INVALID_ARG;

  for (int i = 0; i < MANAGE->mqtt_num; i++)
  {
    if (client_in == MANAGE->clients[i]->client)
    {
      if (MANAGE->clients[i]->device_id != NULL)
        free(MANAGE->clients[i]->device_id);
      int dev_id_size = strlen(dev_id_in) + 1;
      MANAGE->clients[i]->device_id = malloc(dev_id_size);
      memcpy(MANAGE->clients[i]->device_id, dev_id_in, dev_id_size - 1);
      MANAGE->clients[i]->device_id[dev_id_size] = '\0';
      return ESP_OK;
    }
  }

  return ESP_ERR_INVALID_ARG;
}

esp_err_t mt_mqtt_manage_get_session_id(esp_mqtt_client_handle_t *client_in,
                                        uint64_t *session_id_out)
{
  if (MANAGE == NULL)
    return ESP_ERR_INVALID_ARG;

  for (int i = 0; i < MANAGE->mqtt_num; i++)
  {
    if (MANAGE->clients[i]->client == client_in)
    {
      *session_id_out = MANAGE->clients[i]->session_id;
      return ESP_OK;
    }
  }

  return ESP_ERR_INVALID_ARG;
}

esp_err_t mt_mqtt_manage_set_session_id(esp_mqtt_client_handle_t *client_in,
                                        uint64_t sess_id_in)
{
  if (MANAGE == NULL)
    return ESP_ERR_INVALID_ARG;

  for (int i = 0; i < MANAGE->mqtt_num; i++)
  {
    if (client_in == MANAGE->clients[i]->client)
    {
      MANAGE->clients[i]->session_id = sess_id_in;
      return ESP_OK;
    }
  }

  return ESP_ERR_INVALID_ARG;
}
