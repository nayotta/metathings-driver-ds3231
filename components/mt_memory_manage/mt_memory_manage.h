#ifndef _MT_MEMORY_MANAGE_H
#define _MT_MEMORY_MANAGE_H

#include "esp_system.h"

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

void mt_memory_manage_task(bool debug);

#endif
