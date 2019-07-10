#include "esp_system.h"

#include "mt_utils_session.h"

// static func ===============================================================
static uint32_t gen_connection_session_data()
{
    return esp_random() & (uint32_t)x_CONNECTION_SESSION_DATA_MASK |
           SESSION_CODE_FLAG;
}

// global func ================================================================
uint32_t mt_utils_session_gen_startup_session() { return esp_random(); }

uint32_t mt_utils_session_gen_major_session()
{
    return gen_connection_session_data() | MAJOR_SESSION_FLAG;
}

uint32_t mt_utils_session_gen_minor_session()
{
    return gen_connection_session_data() | MINOR_SESSION_FLAG;
}

uint32_t mt_utils_session_gen_temp_session()
{
    return gen_connection_session_data() | TEMP_SESSION_FLAG;
}

uint32_t mt_utils_session_get_startup_session(uint64_t s)
{
    return (uint32_t)((s & STARTUP_SESSION_MASK) >> 32);
}

uint32_t mt_utils_session_get_connection_session(uint64_t s)
{
    return (uint32_t)(s & STARTUP_SESSION_MASK);
}

bool mt_utils_session_is_connection_session(uint32_t s)
{
    return s & x_CONNECTION_SESSION_CODE_MASK == x_CONNECTION_SESSION_CODE_MASK;
}

bool mt_utils_session_is_major_session(uint32_t s)
{
    return mt_utils_session_is_connection_session(s) &&
           (s & x_CONNECTION_SESSION_TYPE_MASK == MAJOR_SESSION_FLAG);
}

bool mt_utils_session_is_major_session_64(uint64_t s)
{
    return mt_utils_session_is_major_session(mt_utils_session_get_connection_session(s));
}

bool mt_utils_session_is_minor_session(uint32_t s)
{
    return mt_utils_session_is_connection_session(s) &&
           (s & x_CONNECTION_SESSION_TYPE_MASK == MINOR_SESSION_FLAG);
}

bool mt_utils_session_is_minor_session_64(uint64_t s)
{
    return mt_utils_session_is_minor_session(mt_utils_session_get_connection_session(s));
}

bool mt_utils_session_is_temp_session(uint32_t s)
{
    return mt_utils_session_is_connection_session(s) &&
           (s & x_CONNECTION_SESSION_TEMP_MASK == TEMP_SESSION_FLAG);
}

uint64_t mt_utils_session_new_session(uint32_t startup, uint32_t conn)
{
    return (uint64_t)(startup) << 32 + (uint64_t)conn;
}
