/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: mt_proto_sys_monitor.proto */

#ifndef PROTOBUF_C_mt_5fproto_5fsys_5fmonitor_2eproto__INCLUDED
#define PROTOBUF_C_mt_5fproto_5fsys_5fmonitor_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1000000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1002001 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif

#include "google/protobuf/empty.pb-c.h"

typedef struct _MtSysMonitor__State MtSysMonitor__State;
typedef struct _MtSysMonitor__GetStateRes MtSysMonitor__GetStateRes;
typedef struct _MtSysMonitor__SetRestartRes MtSysMonitor__SetRestartRes;


/* --- enums --- */


/* --- messages --- */

struct  _MtSysMonitor__State
{
  ProtobufCMessage base;
  /*
   * 启动到现在的时间按 单位秒
   */
  protobuf_c_boolean has_startup;
  int32_t startup;
  /*
   * 重启计数
   */
  protobuf_c_boolean has_restartcount;
  int32_t restartcount;
  /*
   * 空闲内存百分比
   */
  protobuf_c_boolean has_free;
  float free;
  /*
   * 错误计数器
   */
  protobuf_c_boolean has_errorcount;
  int32_t errorcount;
};
#define MT_SYS_MONITOR__STATE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_sys_monitor__state__descriptor) \
    , 0,0, 0,0, 0,0, 0,0 }


struct  _MtSysMonitor__GetStateRes
{
  ProtobufCMessage base;
  protobuf_c_boolean has_code;
  int32_t code;
  MtSysMonitor__State *state;
};
#define MT_SYS_MONITOR__GET_STATE_RES__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_sys_monitor__get_state_res__descriptor) \
    , 0,0, NULL }


struct  _MtSysMonitor__SetRestartRes
{
  ProtobufCMessage base;
  protobuf_c_boolean has_code;
  int32_t code;
};
#define MT_SYS_MONITOR__SET_RESTART_RES__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_sys_monitor__set_restart_res__descriptor) \
    , 0,0 }


/* MtSysMonitor__State methods */
void   mt_sys_monitor__state__init
                     (MtSysMonitor__State         *message);
size_t mt_sys_monitor__state__get_packed_size
                     (const MtSysMonitor__State   *message);
size_t mt_sys_monitor__state__pack
                     (const MtSysMonitor__State   *message,
                      uint8_t             *out);
size_t mt_sys_monitor__state__pack_to_buffer
                     (const MtSysMonitor__State   *message,
                      ProtobufCBuffer     *buffer);
MtSysMonitor__State *
       mt_sys_monitor__state__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_sys_monitor__state__free_unpacked
                     (MtSysMonitor__State *message,
                      ProtobufCAllocator *allocator);
/* MtSysMonitor__GetStateRes methods */
void   mt_sys_monitor__get_state_res__init
                     (MtSysMonitor__GetStateRes         *message);
size_t mt_sys_monitor__get_state_res__get_packed_size
                     (const MtSysMonitor__GetStateRes   *message);
size_t mt_sys_monitor__get_state_res__pack
                     (const MtSysMonitor__GetStateRes   *message,
                      uint8_t             *out);
size_t mt_sys_monitor__get_state_res__pack_to_buffer
                     (const MtSysMonitor__GetStateRes   *message,
                      ProtobufCBuffer     *buffer);
MtSysMonitor__GetStateRes *
       mt_sys_monitor__get_state_res__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_sys_monitor__get_state_res__free_unpacked
                     (MtSysMonitor__GetStateRes *message,
                      ProtobufCAllocator *allocator);
/* MtSysMonitor__SetRestartRes methods */
void   mt_sys_monitor__set_restart_res__init
                     (MtSysMonitor__SetRestartRes         *message);
size_t mt_sys_monitor__set_restart_res__get_packed_size
                     (const MtSysMonitor__SetRestartRes   *message);
size_t mt_sys_monitor__set_restart_res__pack
                     (const MtSysMonitor__SetRestartRes   *message,
                      uint8_t             *out);
size_t mt_sys_monitor__set_restart_res__pack_to_buffer
                     (const MtSysMonitor__SetRestartRes   *message,
                      ProtobufCBuffer     *buffer);
MtSysMonitor__SetRestartRes *
       mt_sys_monitor__set_restart_res__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_sys_monitor__set_restart_res__free_unpacked
                     (MtSysMonitor__SetRestartRes *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*MtSysMonitor__State_Closure)
                 (const MtSysMonitor__State *message,
                  void *closure_data);
typedef void (*MtSysMonitor__GetStateRes_Closure)
                 (const MtSysMonitor__GetStateRes *message,
                  void *closure_data);
typedef void (*MtSysMonitor__SetRestartRes_Closure)
                 (const MtSysMonitor__SetRestartRes *message,
                  void *closure_data);

/* --- services --- */

typedef struct _MtSysMonitor__MtSysMonitor_Service MtSysMonitor__MtSysMonitor_Service;
struct _MtSysMonitor__MtSysMonitor_Service
{
  ProtobufCService base;
  void (*sys_monitor_get_state)(MtSysMonitor__MtSysMonitor_Service *service,
                                const Google__Protobuf__Empty *input,
                                MtSysMonitor__GetStateRes_Closure closure,
                                void *closure_data);
  void (*sys_monitor_set_restart)(MtSysMonitor__MtSysMonitor_Service *service,
                                  const Google__Protobuf__Empty *input,
                                  MtSysMonitor__SetRestartRes_Closure closure,
                                  void *closure_data);
};
typedef void (*MtSysMonitor__MtSysMonitor_ServiceDestroy)(MtSysMonitor__MtSysMonitor_Service *);
void mt_sys_monitor__mt_sys_monitor__init (MtSysMonitor__MtSysMonitor_Service *service,
                                           MtSysMonitor__MtSysMonitor_ServiceDestroy destroy);
#define MT_SYS_MONITOR__MT_SYS_MONITOR__BASE_INIT \
    { &mt_sys_monitor__mt_sys_monitor__descriptor, protobuf_c_service_invoke_internal, NULL }
#define MT_SYS_MONITOR__MT_SYS_MONITOR__INIT(function_prefix__) \
    { MT_SYS_MONITOR__MT_SYS_MONITOR__BASE_INIT,\
      function_prefix__ ## sys_monitor_get_state,\
      function_prefix__ ## sys_monitor_set_restart  }
void mt_sys_monitor__mt_sys_monitor__sys_monitor_get_state(ProtobufCService *service,
                                                           const Google__Protobuf__Empty *input,
                                                           MtSysMonitor__GetStateRes_Closure closure,
                                                           void *closure_data);
void mt_sys_monitor__mt_sys_monitor__sys_monitor_set_restart(ProtobufCService *service,
                                                             const Google__Protobuf__Empty *input,
                                                             MtSysMonitor__SetRestartRes_Closure closure,
                                                             void *closure_data);

/* --- descriptors --- */

extern const ProtobufCMessageDescriptor mt_sys_monitor__state__descriptor;
extern const ProtobufCMessageDescriptor mt_sys_monitor__get_state_res__descriptor;
extern const ProtobufCMessageDescriptor mt_sys_monitor__set_restart_res__descriptor;
extern const ProtobufCServiceDescriptor mt_sys_monitor__mt_sys_monitor__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_mt_5fproto_5fsys_5fmonitor_2eproto__INCLUDED */