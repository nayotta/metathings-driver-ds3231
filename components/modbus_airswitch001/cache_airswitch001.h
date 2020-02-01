#ifndef _CAHCE_AIRSWITCH001_H
#define _CAHCE_AIRSWITCH001_H

#include "esp_err.h"
#include "mt_port_airswitch001.h"
#include "stdbool.h"
#include "stdio.h"

struct cache_t {
  int32_t num_master;
  int32_t *masters;
  int32_t num_slaver;
  int32_t *slavers;
};

typedef struct cache_t Cache_t;

esp_err_t cache_get(Cache_t *cache);

esp_err_t cache_set(Cache_t *cache);

bool cahce_diff(Cache_t *cache1, Cache_t *cache2);

void cache_from_modbus(UCHAR slaveAddr, Cache_t *cache);

esp_err_t cache_get_and_check(Cache_t *cache, int num_master, int num_slaver);

int *cache_convert(Cache_t *cache, int *num);

esp_err_t cache_get_target(int addr_in, UCHAR *target);

esp_err_t cache_task();

esp_err_t cache_quality_get(int addr, int32_t *quality, int32_t *quality_save);

esp_err_t cache_quality_set(int addr, int32_t quality);

#endif
