#ifndef _MT_UTILS_SESSION_H
#define _MT_UTILS_SESSION_H

#include "stdint.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
#endif

#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 100
#endif

uint32_t x_CONNECTION_SESSION_CODE_MASK = (uint32_t)0x40 << 24;
uint32_t x_CONNECTION_SESSION_TYPE_MASK = (uint32_t)0x20 << 24;
uint32_t x_CONNECTION_SESSION_TEMP_MASK = (uint32_t)0x10 << 24;

uint32_t SESSION_CODE_FLAG = (uint32_t)0x40 << 24;
uint32_t MAJOR_SESSION_FLAG = (uint32_t)0x00;
uint32_t MINOR_SESSION_FLAG = (uint32_t)0x20 << 24;
uint32_t TEMP_SESSION_FLAG = (uint32_t)0x10 << 24;
uint64_t STARTUP_SESSION_MASK = 0x7fffffff00000000;
uint64_t CONNECTION_SESSION_MASK = 0x000000007fffffff;
uint64_t x_CONNECTION_SESSION_DATA_MASK = 0x0000000000ffffff;

uint32_t mt_utils_session_gen_startup_session();

uint32_t mt_utils_session_gen_major_session();

uint32_t mt_utils_session_gen_minor_session();

uint32_t mt_utils_session_gen_temp_session(uint64_t s);

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