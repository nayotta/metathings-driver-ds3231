/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: mt_proto_sys_monitor.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "mt_proto_sys_monitor.pb-c.h"
void   mt_sys_monitor__state__init
                     (MtSysMonitor__State         *message)
{
  static MtSysMonitor__State init_value = MT_SYS_MONITOR__STATE__INIT;
  *message = init_value;
}
size_t mt_sys_monitor__state__get_packed_size
                     (const MtSysMonitor__State *message)
{
  assert(message->base.descriptor == &mt_sys_monitor__state__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t mt_sys_monitor__state__pack
                     (const MtSysMonitor__State *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &mt_sys_monitor__state__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t mt_sys_monitor__state__pack_to_buffer
                     (const MtSysMonitor__State *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &mt_sys_monitor__state__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
MtSysMonitor__State *
       mt_sys_monitor__state__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (MtSysMonitor__State *)
     protobuf_c_message_unpack (&mt_sys_monitor__state__descriptor,
                                allocator, len, data);
}
void   mt_sys_monitor__state__free_unpacked
                     (MtSysMonitor__State *message,
                      ProtobufCAllocator *allocator)
{
  assert(message->base.descriptor == &mt_sys_monitor__state__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   mt_sys_monitor__get_state_res__init
                     (MtSysMonitor__GetStateRes         *message)
{
  static MtSysMonitor__GetStateRes init_value = MT_SYS_MONITOR__GET_STATE_RES__INIT;
  *message = init_value;
}
size_t mt_sys_monitor__get_state_res__get_packed_size
                     (const MtSysMonitor__GetStateRes *message)
{
  assert(message->base.descriptor == &mt_sys_monitor__get_state_res__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t mt_sys_monitor__get_state_res__pack
                     (const MtSysMonitor__GetStateRes *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &mt_sys_monitor__get_state_res__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t mt_sys_monitor__get_state_res__pack_to_buffer
                     (const MtSysMonitor__GetStateRes *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &mt_sys_monitor__get_state_res__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
MtSysMonitor__GetStateRes *
       mt_sys_monitor__get_state_res__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (MtSysMonitor__GetStateRes *)
     protobuf_c_message_unpack (&mt_sys_monitor__get_state_res__descriptor,
                                allocator, len, data);
}
void   mt_sys_monitor__get_state_res__free_unpacked
                     (MtSysMonitor__GetStateRes *message,
                      ProtobufCAllocator *allocator)
{
  assert(message->base.descriptor == &mt_sys_monitor__get_state_res__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   mt_sys_monitor__set_restart_res__init
                     (MtSysMonitor__SetRestartRes         *message)
{
  static MtSysMonitor__SetRestartRes init_value = MT_SYS_MONITOR__SET_RESTART_RES__INIT;
  *message = init_value;
}
size_t mt_sys_monitor__set_restart_res__get_packed_size
                     (const MtSysMonitor__SetRestartRes *message)
{
  assert(message->base.descriptor == &mt_sys_monitor__set_restart_res__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t mt_sys_monitor__set_restart_res__pack
                     (const MtSysMonitor__SetRestartRes *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &mt_sys_monitor__set_restart_res__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t mt_sys_monitor__set_restart_res__pack_to_buffer
                     (const MtSysMonitor__SetRestartRes *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &mt_sys_monitor__set_restart_res__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
MtSysMonitor__SetRestartRes *
       mt_sys_monitor__set_restart_res__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (MtSysMonitor__SetRestartRes *)
     protobuf_c_message_unpack (&mt_sys_monitor__set_restart_res__descriptor,
                                allocator, len, data);
}
void   mt_sys_monitor__set_restart_res__free_unpacked
                     (MtSysMonitor__SetRestartRes *message,
                      ProtobufCAllocator *allocator)
{
  assert(message->base.descriptor == &mt_sys_monitor__set_restart_res__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor mt_sys_monitor__state__field_descriptors[4] =
{
  {
    "startup",
    1,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_INT32,
    offsetof(MtSysMonitor__State, has_startup),
    offsetof(MtSysMonitor__State, startup),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "restartcount",
    2,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_INT32,
    offsetof(MtSysMonitor__State, has_restartcount),
    offsetof(MtSysMonitor__State, restartcount),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "free",
    3,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_FLOAT,
    offsetof(MtSysMonitor__State, has_free),
    offsetof(MtSysMonitor__State, free),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "errorcount",
    4,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_INT32,
    offsetof(MtSysMonitor__State, has_errorcount),
    offsetof(MtSysMonitor__State, errorcount),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned mt_sys_monitor__state__field_indices_by_name[] = {
  3,   /* field[3] = errorcount */
  2,   /* field[2] = free */
  1,   /* field[1] = restartcount */
  0,   /* field[0] = startup */
};
static const ProtobufCIntRange mt_sys_monitor__state__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 4 }
};
const ProtobufCMessageDescriptor mt_sys_monitor__state__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "mt_sys_monitor.State",
  "State",
  "MtSysMonitor__State",
  "mt_sys_monitor",
  sizeof(MtSysMonitor__State),
  4,
  mt_sys_monitor__state__field_descriptors,
  mt_sys_monitor__state__field_indices_by_name,
  1,  mt_sys_monitor__state__number_ranges,
  (ProtobufCMessageInit) mt_sys_monitor__state__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor mt_sys_monitor__get_state_res__field_descriptors[2] =
{
  {
    "code",
    1,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_INT32,
    offsetof(MtSysMonitor__GetStateRes, has_code),
    offsetof(MtSysMonitor__GetStateRes, code),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "state",
    2,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(MtSysMonitor__GetStateRes, state),
    &mt_sys_monitor__state__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned mt_sys_monitor__get_state_res__field_indices_by_name[] = {
  0,   /* field[0] = code */
  1,   /* field[1] = state */
};
static const ProtobufCIntRange mt_sys_monitor__get_state_res__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 2 }
};
const ProtobufCMessageDescriptor mt_sys_monitor__get_state_res__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "mt_sys_monitor.GetStateRes",
  "GetStateRes",
  "MtSysMonitor__GetStateRes",
  "mt_sys_monitor",
  sizeof(MtSysMonitor__GetStateRes),
  2,
  mt_sys_monitor__get_state_res__field_descriptors,
  mt_sys_monitor__get_state_res__field_indices_by_name,
  1,  mt_sys_monitor__get_state_res__number_ranges,
  (ProtobufCMessageInit) mt_sys_monitor__get_state_res__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor mt_sys_monitor__set_restart_res__field_descriptors[1] =
{
  {
    "code",
    1,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_INT32,
    offsetof(MtSysMonitor__SetRestartRes, has_code),
    offsetof(MtSysMonitor__SetRestartRes, code),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned mt_sys_monitor__set_restart_res__field_indices_by_name[] = {
  0,   /* field[0] = code */
};
static const ProtobufCIntRange mt_sys_monitor__set_restart_res__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 1 }
};
const ProtobufCMessageDescriptor mt_sys_monitor__set_restart_res__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "mt_sys_monitor.SetRestartRes",
  "SetRestartRes",
  "MtSysMonitor__SetRestartRes",
  "mt_sys_monitor",
  sizeof(MtSysMonitor__SetRestartRes),
  1,
  mt_sys_monitor__set_restart_res__field_descriptors,
  mt_sys_monitor__set_restart_res__field_indices_by_name,
  1,  mt_sys_monitor__set_restart_res__number_ranges,
  (ProtobufCMessageInit) mt_sys_monitor__set_restart_res__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCMethodDescriptor mt_sys_monitor__mt_sys_monitor__method_descriptors[2] =
{
  { "SysMonitorGetState", &google__protobuf__empty__descriptor, &mt_sys_monitor__get_state_res__descriptor },
  { "SysMonitorSetRestart", &google__protobuf__empty__descriptor, &mt_sys_monitor__set_restart_res__descriptor },
};
const unsigned mt_sys_monitor__mt_sys_monitor__method_indices_by_name[] = {
  0,        /* SysMonitorGetState */
  1         /* SysMonitorSetRestart */
};
const ProtobufCServiceDescriptor mt_sys_monitor__mt_sys_monitor__descriptor =
{
  PROTOBUF_C__SERVICE_DESCRIPTOR_MAGIC,
  "mt_sys_monitor.MtSysMonitor",
  "MtSysMonitor",
  "MtSysMonitor__MtSysMonitor",
  "mt_sys_monitor",
  2,
  mt_sys_monitor__mt_sys_monitor__method_descriptors,
  mt_sys_monitor__mt_sys_monitor__method_indices_by_name
};
void mt_sys_monitor__mt_sys_monitor__sys_monitor_get_state(ProtobufCService *service,
                                                           const Google__Protobuf__Empty *input,
                                                           MtSysMonitor__GetStateRes_Closure closure,
                                                           void *closure_data)
{
  assert(service->descriptor == &mt_sys_monitor__mt_sys_monitor__descriptor);
  service->invoke(service, 0, (const ProtobufCMessage *) input, (ProtobufCClosure) closure, closure_data);
}
void mt_sys_monitor__mt_sys_monitor__sys_monitor_set_restart(ProtobufCService *service,
                                                             const Google__Protobuf__Empty *input,
                                                             MtSysMonitor__SetRestartRes_Closure closure,
                                                             void *closure_data)
{
  assert(service->descriptor == &mt_sys_monitor__mt_sys_monitor__descriptor);
  service->invoke(service, 1, (const ProtobufCMessage *) input, (ProtobufCClosure) closure, closure_data);
}
void mt_sys_monitor__mt_sys_monitor__init (MtSysMonitor__MtSysMonitor_Service *service,
                                           MtSysMonitor__MtSysMonitor_ServiceDestroy destroy)
{
  protobuf_c_service_generated_init (&service->base,
                                     &mt_sys_monitor__mt_sys_monitor__descriptor,
                                     (ProtobufCServiceDestroy) destroy);
}
