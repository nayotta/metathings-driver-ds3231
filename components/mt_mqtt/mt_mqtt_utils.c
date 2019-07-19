#include "string.h"

#include "esp_log.h"

#include "mt_mqtt_utils.h"

// global value ===============================================================
static const char *TAG = "MT_MQTT_UTILS";

// global func ================================================================
esp_err_t mt_mqtt_utils_get_session_id_from_topic(char *topic,
                                                  uint64_t *session_id)
{
  // mt/modules/%s/proxy/sessions/%s/upstream/

  int offset = 0;
  int start = 0;
  int i = 0;
  int split_count = 0;
  int min_split_size = 5;
  char s1[64] = "";
  char s2[64] = "";
  char s3[64] = "";
  char s4[64] = "";
  char s5[64] = "";
  char s6[64] = "";

  for (i = 0; i < strlen(topic); i++)
  {
    if (topic[i] == '/')
    {
      split_count++;
    }
  }
  if (split_count < min_split_size)
  {
    ESP_LOGE(TAG, "%4d %s topic size split less than %d", __LINE__, __func__,
             min_split_size);
    return ESP_ERR_INVALID_ARG;
  }

  while (topic[offset] != '/' && offset < strlen(topic) - 1)
  {
    s1[start] = topic[offset];
    start++;
    offset++;
  }
  s1[start] = '\0';
  if (strcmp(s1, "mt") != 0)
  {
    ESP_LOGE(TAG, "%4d %s unexcept string location 1:%s", __LINE__, __func__,
             s1);
    return ESP_ERR_INVALID_ARG;
  }

  start = 0;
  offset++;
  while (topic[offset] != '/' && offset < strlen(topic) - 1)
  {
    s2[start] = topic[offset];
    start++;
    offset++;
  }
  s2[start] = '\0';
  if (strcmp(s2, "modules") != 0)
  {
    ESP_LOGE(TAG, "%4d %s unexcept string location 2:%s", __LINE__, __func__,
             s2);
    return ESP_ERR_INVALID_ARG;
  }

  start = 0;
  offset++;
  while (topic[offset] != '/' && offset < strlen(topic) - 1)
  {
    s3[start] = topic[offset];
    start++;
    offset++;
  }
  s3[start] = '\0';
  if (strlen(s3) == 0)
  {
    ESP_LOGE(TAG, "%4d %s unexcept string location 3:%s", __LINE__, __func__,
             s3);
    return ESP_ERR_INVALID_ARG;
  }

  start = 0;
  offset++;
  while (topic[offset] != '/' && offset < strlen(topic) - 1)
  {
    s4[start] = topic[offset];
    start++;
    offset++;
  }
  s4[start] = '\0';
  if (strcmp(s4, "proxy") != 0)
  {
    ESP_LOGE(TAG, "%4d %s unexcept string location 4:%s", __LINE__, __func__,
             s4);
    return ESP_ERR_INVALID_ARG;
  }

  start = 0;
  offset++;
  while (topic[offset] != '/' && offset < strlen(topic) - 1)
  {
    s5[start] = topic[offset];
    start++;
    offset++;
  }
  s5[start] = '\0';
  if (strcmp(s5, "sessions") != 0)
  {
    ESP_LOGE(TAG, "%4d %s unexcept string location 5:%s", __LINE__, __func__,
             s5);
    return ESP_ERR_INVALID_ARG;
  }

  start = 0;
  offset++;
  while (topic[offset] != '/' && offset < strlen(topic) - 1)
  {
    s6[start] = topic[offset];
    start++;
    offset++;
  }
  s6[start] = '\0';

  if (sscanf(s6, "%llu", session_id) < 1)
  {
    ESP_LOGE(TAG, "%4d %s failed to get session_id", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (*session_id == 0)
  {
    ESP_LOGE(TAG, "%4d %s get zero session_id", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}

char *mt_mqtt_utils_get_path_from_topic(char *topic)
{
  // mt/modules/%s/proxy/sessions/%s/upstream/
  uint8_t i = 0;
  uint8_t split_count = 0;
  uint8_t min_split_size = 3;
  uint8_t offset = 0;
  uint8_t start = 0;
  char s1[64] = "";
  char s2[64] = "";
  char s3[64] = "";
  char s4[64] = "";
  char *path_out = NULL;

  for (i = 0; i < strlen(topic); i++)
  {
    if (topic[i] == '/')
    {
      split_count++;
    }
  }
  if (split_count < min_split_size)
  {
    ESP_LOGE(TAG, "%4d %s topic size split less than %d", __LINE__, __func__,
             min_split_size);
    return NULL;
  }

  while (topic[offset] != '/' && offset < strlen(topic) - 1)
  {
    s1[start] = topic[offset];
    start++;
    offset++;
  }
  s1[start] = '\0';
  if (strcmp(s1, "mt") != 0)
  {
    ESP_LOGE(TAG, "%4d %s unexcept string location 1:%s", __LINE__, __func__,
             s1);
    return NULL;
  }

  start = 0;
  offset++;
  while (topic[offset] != '/' && offset < strlen(topic) - 1)
  {
    s2[start] = topic[offset];
    start++;
    offset++;
  }
  s2[start] = '\0';
  if (strcmp(s2, "modules") != 0)
  {
    ESP_LOGE(TAG, "%4d %s unexcept string location 2:%s", __LINE__, __func__,
             s2);
    return NULL;
  }

  start = 0;
  offset++;
  while (topic[offset] != '/' && offset < strlen(topic) - 1)
  {
    s3[start] = topic[offset];
    start++;
    offset++;
  }
  s3[start] = '\0';
  if (strlen(s3) == 0)
  {
    ESP_LOGE(TAG, "%4d %s unexcept string location 3:%s", __LINE__, __func__,
             s3);
    return NULL;
  }

  start = 0;
  offset++;
  while (topic[offset] != '/' && offset < strlen(topic) - 1)
  {
    s4[start] = topic[offset];
    start++;
    offset++;
  }
  s4[start] = '\0';

  path_out = malloc(start);
  path_out[start - 1] = '\0';
  memcpy(path_out, s4, start - 1);

  return path_out;
}
