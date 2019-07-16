#ifndef _MT_UTILS_SESSION_H
#define _MT_UTILS_SESSION_H

#include "stdint.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

uint32_t mt_utils_session_gen_startup_session();

uint32_t mt_utils_session_gen_major_session();

uint32_t mt_utils_session_gen_minor_session();

uint32_t mt_utils_session_gen_temp_session();

uint32_t mt_utils_session_get_startup_session(uint64_t s);

uint32_t mt_utils_session_get_connection_session(uint64_t s);

bool mt_utils_session_is_connection_session(uint32_t s);

bool mt_utils_session_is_major_session(uint32_t s);

bool mt_utils_session_is_major_session_64(uint64_t s);

bool mt_utils_session_is_minor_session(uint32_t s);

bool mt_utils_session_is_minor_session_64(uint64_t s);

bool mt_utils_session_is_temp_session(uint32_t s);

uint64_t mt_utils_session_new_session(uint32_t startup, uint32_t conn);

#endif