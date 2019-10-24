#include "stdio.h"
#include "string.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "cache_airswitch001.h"
#include "modbus_airswitch001.h"
#include "mt_nvs_storage.h"
#include "mt_port_airswitch001.h"

// global config ==============================================================
static const char *TAG = "CACHE_airswitch001";

int NUM_MASTER = 1;
int NUM_SLAVER = 2;

#define SWITCH_MAX 16
bool SWITCH_EXIST[SWITCH_MAX];

// static func ================================================================
static esp_err_t cache_quality_update(int addr, int32_t quality_in)
{
  char key[16] = "";
  int32_t quality = 0;
  int32_t quality_save = 0;
  int32_t update_value = 0;

  if (cache_quality_get(addr, &quality, &quality_save) != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s cache_quality_get failed", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (quality_in > quality_save)
  {
    update_value = quality_in - quality_save;
  }
  else if (quality_in < quality_save)
  {
    update_value = quality_in;
  }
  else
  {
    return ESP_OK;
  }

  sprintf(key, "qual_%d_save", addr);
  if (mt_nvs_write_int32_config(key, quality_in) == false)
  {
    ESP_LOGE(TAG, "%4d %s cache write failed", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  sprintf(key, "qual_%d", addr);
  if (mt_nvs_write_int32_config(key, quality + update_value) == false)
  {
    ESP_LOGE(TAG, "%4d %s cache write failed", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}

static void quality_cache_loop()
{
  vTaskDelay(10 * 1000 / portTICK_RATE_MS);
  while (true)
  {
    esp_err_t err = ESP_OK;
    int32_t quality = 0;
    int num = NUM_SLAVER + NUM_MASTER;

    for (int i = 1; i <= num; i++)
    {
      USHORT value_high = 0;
      USHORT value_low = 0;

      err = mt_airswitch001_get_data(1, DATA_QUALITY_H, i, &value_high);
      if (err != ESP_OK)
      {
        ESP_LOGE(TAG, "%4d %s cache failed", __LINE__, __func__);
        goto CONTINUE;
      }

      err = mt_airswitch001_get_data(1, DATA_QUALITY_L, i, &value_low);
      if (err != ESP_OK)
      {
        ESP_LOGE(TAG, "%4d %s cache failed", __LINE__, __func__);
        goto CONTINUE;
      }

      quality = (value_high << 8) + value_low;
      err = cache_quality_update(i, quality);
      if (err != ESP_OK)
      {
        ESP_LOGE(TAG, "%4d %s cache_quality_update failed", __LINE__, __func__);
        goto CONTINUE;
      }

    CONTINUE:
      vTaskDelay(30 * 1000 / portTICK_RATE_MS);
    }
  }
}

// global func ================================================================
static void cache_print(Cache_t *cache)
{
  ESP_LOGI(TAG, "%4d %s cache num_master:%d num_slaver:%d", __LINE__, __func__,
           cache->num_master, cache->num_slaver);
  for (int i = 0; i < cache->num_master; i++)
  {
    ESP_LOGI(TAG, "%4d %s cache master addr:%d", __LINE__, __func__,
             cache->masters[i]);
  }
  for (int i = 0; i < cache->num_slaver; i++)
  {
    ESP_LOGI(TAG, "%4d %s cache slaver addr:%d", __LINE__, __func__,
             cache->slavers[i]);
  }
}

esp_err_t cache_get(Cache_t *cache)
{
  bool bool_ret = false;
  char key[128];
  int i;

  bool_ret = mt_nvs_read_int32_config("num_master", &cache->num_master);
  if (bool_ret == false)
  {
    ESP_LOGE(TAG, "%4d %s get num_master failed", __LINE__, __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cache->num_master <= 0)
  {
    ESP_LOGE(TAG, "%4d %s num_master:%d error", __LINE__, __func__,
             cache->num_master);
    return ESP_ERR_INVALID_RESPONSE;
  }

  cache->masters = malloc(cache->num_master * sizeof(int));

  for (i = 0; i < cache->num_master; i++)
  {
    sprintf(key, "master%d", i);
    bool_ret = mt_nvs_read_int32_config(key, &cache->masters[i]);
    if (bool_ret == false)
    {
      ESP_LOGE(TAG, "%4d %s get master[%d] failed", __LINE__, __func__, i);
      return ESP_ERR_INVALID_RESPONSE;
    }
  }

  bool_ret = mt_nvs_read_int32_config("num_slaver", &cache->num_slaver);
  if (bool_ret == false)
  {
    ESP_LOGE(TAG, "%4d %s get num_slaver failed", __LINE__, __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (cache->num_slaver <= 0)
  {
    ESP_LOGE(TAG, "%4d %s num_slaver:%d error", __LINE__, __func__,
             cache->num_slaver);
    return ESP_ERR_INVALID_RESPONSE;
  }

  cache->slavers = malloc(cache->num_slaver * sizeof(int));

  for (i = 0; i < cache->num_slaver; i++)
  {
    sprintf(key, "slaver%d", i);
    bool_ret = mt_nvs_read_int32_config(key, &cache->slavers[i]);
    if (bool_ret == false)
    {
      ESP_LOGE(TAG, "%4d %s get slaver[%d] failed", __LINE__, __func__, i);
      return ESP_ERR_INVALID_RESPONSE;
    }
  }

  return ESP_OK;
}

esp_err_t cache_set(Cache_t *cache)
{
  bool bool_ret = false;
  char key[128];
  int i;

  cache_print(cache);

  if (cache->num_master < 0)
  {
    ESP_LOGE(TAG, "%4d %s num_master:%d error", __LINE__, __func__,
             cache->num_master);
    return ESP_ERR_INVALID_RESPONSE;
  }

  bool_ret = mt_nvs_write_int32_config("num_master", cache->num_master);
  if (bool_ret == false)
  {
    ESP_LOGE(TAG, "%4d %s set num_master failed", __LINE__, __func__);
    return ESP_ERR_INVALID_RESPONSE;
  }

  for (i = 0; i < cache->num_master; i++)
  {
    sprintf(key, "master%d", i);
    bool_ret = mt_nvs_write_int32_config(key, cache->masters[i]);
    if (bool_ret == false)
    {
      ESP_LOGE(TAG, "%4d %s set master:%d failed", __LINE__, __func__, i);
      return ESP_ERR_INVALID_RESPONSE;
    }
  }

  if (cache->num_slaver < 0)
  {
    ESP_LOGE(TAG, "%4d %s num_slaver:%d error", __LINE__, __func__,
             cache->num_slaver);
    return ESP_ERR_INVALID_RESPONSE;
  }

  bool_ret = mt_nvs_write_int32_config("num_slaver", cache->num_slaver);
  if (bool_ret == false)
  {
    ESP_LOGE(TAG, "%4d %s set num_slaver:%d failed", __LINE__, __func__,
             cache->num_slaver);
    return ESP_ERR_INVALID_RESPONSE;
  }

  for (i = 0; i < cache->num_slaver; i++)
  {
    sprintf(key, "slaver%d", i);
    bool_ret = mt_nvs_write_int32_config(key, cache->slavers[i]);
    if (bool_ret == false)
    {
      ESP_LOGE(TAG, "%4d %s set slaver:%d failed", __LINE__, __func__, i);
      return ESP_ERR_INVALID_RESPONSE;
    }
  }

  ESP_LOGI(TAG, "%4d %s set NUM_MASTER = %d", __LINE__, __func__,
           cache->num_master);
  for (i = 0; i < cache->num_master; i++)
  {
    ESP_LOGI(TAG, "%4d %s set MASTER[%d] addr = %d", __LINE__, __func__, i,
             cache->masters[i]);
  }
  ESP_LOGI(TAG, "%4d %s set NUM_SLAVER = %d", __LINE__, __func__,
           cache->num_slaver);
  for (i = 0; i < cache->num_slaver; i++)
  {
    ESP_LOGI(TAG, "%4d %s set SLAVER[%d] addr = %d", __LINE__, __func__, i,
             cache->slavers[i]);
  }

  return ESP_OK;
}

esp_err_t cache_get_and_check(Cache_t *cache, int num_master, int num_slaver)
{
  esp_err_t err = ESP_OK;

  err = cache_get(cache);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s get cache failed", __LINE__, __func__);
    return err;
  }

  if (cache->num_master >= num_master)
  {
    cache->num_master = num_master;
  }

  if (cache->num_slaver >= num_slaver)
  {
    cache->num_slaver = num_slaver;
  }

  return ESP_OK;
}

bool cahce_diff(Cache_t *cache1, Cache_t *cache2)
{
  int i;

  if (cache1->num_master != cache2->num_master)
  {
    ESP_LOGE(TAG, "%4d %s num_master %d:%d not match", __LINE__, __func__,
             cache1->num_master, cache2->num_master);
    return false;
  }

  for (i = 0; i < cache1->num_master; i++)
  {
    if (cache1->masters[i] != cache2->masters[i])
    {
      ESP_LOGE(TAG, "%4d %s master:%d addr %d:%d not match", __LINE__, __func__,
               i, cache1->masters[i], cache2->masters[i]);
      return false;
    }
  }

  if (cache1->num_slaver != cache2->num_slaver)
  {
    ESP_LOGE(TAG, "%4d %s num_slaver %d:%d not match", __LINE__, __func__,
             cache1->num_slaver, cache2->num_slaver);
    return false;
  }

  for (i = 0; i < cache1->num_slaver; i++)
  {
    if (cache1->slavers[i] != cache2->slavers[i])
    {
      ESP_LOGE(TAG, "%4d %s slaver:%d addr %d:%d not match", __LINE__, __func__,
               i, cache1->slavers[i], cache2->slavers[i]);
      return false;
    }
  }

  return true;
}

void cache_from_modbus(UCHAR slaveAddr, Cache_t *cache)
{
  esp_err_t err = ESP_OK;
  int model = 0;
  int current = 0;
  USHORT addrs = 0;
  int addr_master_save[SWITCH_MAX];
  int addr_slaver_save[SWITCH_MAX];
  int addr_slaver_save_count = 0;
  int addr_master_save_count = 0;

RESTART:
  addr_slaver_save_count = 0;
  addr_master_save_count = 0;
  cache->num_master = 0;
  cache->num_slaver = 0;

  err = mt_airswitch001_get_addrs(slaveAddr, &addrs);
  while (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s slaver:%d get addrs error:%d", __LINE__, __func__,
             slaveAddr, err);
    vTaskDelay(3000 / portTICK_RATE_MS);
    goto RESTART;
  }

  ESP_LOGI(TAG, "%4d %s slave:%d get addrs:%d", __LINE__, __func__, slaveAddr,
           addrs);

  for (int i = 0; i < SWITCH_MAX; i++)
  {
    SWITCH_EXIST[i] = (bool)((addrs & (1 << i)) >> i);

    if (SWITCH_EXIST[i] == true)
    {
      err = mt_airswitch001_get_model_no_cache(slaveAddr, i, &model, &current);
      if (err != ESP_OK)
      {
        ESP_LOGE(TAG, "%4d %s slaver:%d target:%d get model error:%d", __LINE__,
                 __func__, slaveAddr, i, err);
        vTaskDelay(3000 / portTICK_RATE_MS);
        goto RESTART;
      }

      ESP_LOGI(TAG, "ADDR:%d, type=%d", i, model);

      switch (model)
      {
      case 8:
        cache->num_slaver++;
        addr_slaver_save[addr_slaver_save_count++] = i;
        break;
      case 7:
        cache->num_master++;
        addr_master_save[addr_master_save_count++] = i;
        break;
      default:
        ESP_LOGE(TAG, "%4d %s slaver:%d target:%d get unexcept model:%d",
                 __LINE__, __func__, slaveAddr, i, model);
        break;
      }
    }
  }

  if (cache->num_master > 0)
  {
    cache->masters = malloc(cache->num_master * sizeof(int));
  }
  if (cache->num_slaver > 0)
  {
    cache->slavers = malloc(cache->num_slaver * sizeof(int));
  }

  ESP_LOGI(TAG,
           "%4d %s num_master:%d num_slaver:%d, master_save:%d, slaver_save:%d",
           __LINE__, __func__, cache->num_master, cache->num_slaver,
           addr_master_save_count, addr_slaver_save_count);

  for (int i = 0; i < cache->num_master; i++)
  {
    cache->masters[i] = addr_master_save[i];
  }
  for (int i = 0; i < cache->num_slaver; i++)
  {
    cache->slavers[i] = addr_slaver_save[i];
  }

  ESP_LOGI(TAG, "%4d %s slave:%d cache finished", __LINE__, __func__,
           slaveAddr);
}

int *cache_convert(Cache_t *cache, int *num)
{
  *num = cache->num_master + cache->num_slaver;
  int *addr = malloc(*num * sizeof(int));
  for (int i = 0; i < cache->num_master; i++)
  {
    addr[i] = cache->masters[i];
  }

  for (int i = 0; i < cache->num_slaver; i++)
  {
    addr[i + cache->num_master] = cache->slavers[i];
  }

  return addr;
}

static void cache_loop()
{
  esp_err_t err = ESP_OK;
  Cache_t *cache = malloc(sizeof(Cache_t));

RESTART:
  cache_from_modbus(1, cache);

  err = cache_set(cache);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s cache_set failed", __LINE__, __func__);
    vTaskDelay(3000 / portTICK_RATE_MS);
    goto RESTART;
  }

  xTaskCreate((TaskFunction_t)quality_cache_loop, "quality_cache_task",
              8 * 1024, NULL, 10, NULL);

  ESP_LOGI(TAG, "%4d %s cache success", __LINE__, __func__);

  vTaskDelete(NULL);
}

esp_err_t cache_task()
{
  if (mt_nvs_read_int32_config("mod_1_master", &NUM_MASTER) == false)
  {
    ESP_LOGE(TAG, "%4d %s mod_1_master read failed", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  if (mt_nvs_read_int32_config("mod_1_slaver", &NUM_SLAVER) == false)
  {
    ESP_LOGE(TAG, "%4d %s mod_1_slaver read failed", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  ESP_LOGI(TAG, "%4d %s cache task master:%d slaver:%d", __LINE__, __func__,
           NUM_MASTER, NUM_SLAVER);

  vTaskDelay(2000 / portTICK_RATE_MS);
  xTaskCreate((TaskFunction_t)cache_loop, "CACHE_TASK", 8 * 1024, NULL, 10,
              NULL);

  return ESP_OK;
}

esp_err_t cache_get_target(int addr_in, UCHAR *target)
{
  esp_err_t err = ESP_OK;
  Cache_t cache;
  cache.masters = NULL;
  cache.slavers = NULL;

  err = cache_get(&cache);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "%4d %s cache_get failed", __LINE__, __func__);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

  if (addr_in <= NUM_MASTER)
  {
    if (addr_in <= cache.num_master)
    {
      *target = cache.masters[addr_in - 1];
    }
    else
    {
      ESP_LOGE(TAG,
               "%4d %s addr error, addr_in:%d, NUM_MASTER:%d, cache_master:%d",
               __LINE__, __func__, addr_in, NUM_MASTER, cache.num_master);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }
  else if (addr_in <= NUM_MASTER + NUM_SLAVER)
  {
    if (addr_in <= NUM_MASTER + cache.num_slaver)
    {
      *target = cache.slavers[addr_in - NUM_MASTER - 1];
    }
    else
    {
      ESP_LOGE(TAG,
               "%4d %s addr error, addr_in:%d, NUM_MASTER:%d, NUM_SLAVER:%d, "
               "cache_slaver:%d",
               __LINE__, __func__, addr_in, NUM_MASTER, NUM_MASTER,
               cache.num_slaver);
      err = ESP_ERR_INVALID_ARG;
      goto EXIT;
    }
  }
  else
  {
    ESP_LOGE(TAG, "%4d %s unexcept addr_in:%d, NUM_MASTER:%d, NUM_SLAVER:%d",
             __LINE__, __func__, addr_in, NUM_MASTER, NUM_SLAVER);
    err = ESP_ERR_INVALID_ARG;
    goto EXIT;
  }

EXIT:
  if (cache.masters != NULL)
    free(cache.masters);
  if (cache.slavers != NULL)
    free(cache.slavers);

  return err;
}

esp_err_t cache_quality_get(int addr, int32_t *quality, int32_t *quality_save)
{
  char key[16] = "";

  sprintf(key, "qual_%d", addr);
  if (mt_nvs_read_int32_config(key, quality) == false)
  {
    *quality = 0;
    if (mt_nvs_write_int32_config(key, 0) == false)
    {
      ESP_LOGE(TAG, "%4d %s cache reset faild", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }
  }

  sprintf(key, "qual_%d_save", addr);
  if (mt_nvs_read_int32_config(key, quality_save) == false)
  {
    *quality_save = 0;
    if (mt_nvs_write_int32_config(key, 0) == false)
    {
      ESP_LOGE(TAG, "%4d %s cache reset faild", __LINE__, __func__);
      return ESP_ERR_INVALID_ARG;
    }
  }

  return ESP_OK;
}

esp_err_t cache_quality_set(int addr, int32_t quality)
{
  char key[16];

  sprintf(key, "qual_%d", addr);
  if (mt_nvs_write_int32_config(key, quality) == false)
  {
    ESP_LOGE(TAG, "%4d %s cache set faild", __LINE__, __func__);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}
