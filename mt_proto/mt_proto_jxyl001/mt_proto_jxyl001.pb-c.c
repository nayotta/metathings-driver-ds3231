/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: mt_proto_jxyl001.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "mt_proto_jxyl001.pb-c.h"
void   mt_jxyl001__temp__init
                     (MtJxyl001__Temp         *message)
{
  static const MtJxyl001__Temp init_value = MT_JXYL001__TEMP__INIT;
  *message = init_value;
}
size_t mt_jxyl001__temp__get_packed_size
                     (const MtJxyl001__Temp *message)
{
  assert(message->base.descriptor == &mt_jxyl001__temp__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t mt_jxyl001__temp__pack
                     (const MtJxyl001__Temp *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &mt_jxyl001__temp__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t mt_jxyl001__temp__pack_to_buffer
                     (const MtJxyl001__Temp *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &mt_jxyl001__temp__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
MtJxyl001__Temp *
       mt_jxyl001__temp__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (MtJxyl001__Temp *)
     protobuf_c_message_unpack (&mt_jxyl001__temp__descriptor,
                                allocator, len, data);
}
void   mt_jxyl001__temp__free_unpacked
                     (MtJxyl001__Temp *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &mt_jxyl001__temp__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   mt_jxyl001__fog__init
                     (MtJxyl001__Fog         *message)
{
  static const MtJxyl001__Fog init_value = MT_JXYL001__FOG__INIT;
  *message = init_value;
}
size_t mt_jxyl001__fog__get_packed_size
                     (const MtJxyl001__Fog *message)
{
  assert(message->base.descriptor == &mt_jxyl001__fog__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t mt_jxyl001__fog__pack
                     (const MtJxyl001__Fog *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &mt_jxyl001__fog__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t mt_jxyl001__fog__pack_to_buffer
                     (const MtJxyl001__Fog *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &mt_jxyl001__fog__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
MtJxyl001__Fog *
       mt_jxyl001__fog__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (MtJxyl001__Fog *)
     protobuf_c_message_unpack (&mt_jxyl001__fog__descriptor,
                                allocator, len, data);
}
void   mt_jxyl001__fog__free_unpacked
                     (MtJxyl001__Fog *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &mt_jxyl001__fog__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   mt_jxyl001__data_res__init
                     (MtJxyl001__DataRes         *message)
{
  static const MtJxyl001__DataRes init_value = MT_JXYL001__DATA_RES__INIT;
  *message = init_value;
}
size_t mt_jxyl001__data_res__get_packed_size
                     (const MtJxyl001__DataRes *message)
{
  assert(message->base.descriptor == &mt_jxyl001__data_res__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t mt_jxyl001__data_res__pack
                     (const MtJxyl001__DataRes *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &mt_jxyl001__data_res__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t mt_jxyl001__data_res__pack_to_buffer
                     (const MtJxyl001__DataRes *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &mt_jxyl001__data_res__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
MtJxyl001__DataRes *
       mt_jxyl001__data_res__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (MtJxyl001__DataRes *)
     protobuf_c_message_unpack (&mt_jxyl001__data_res__descriptor,
                                allocator, len, data);
}
void   mt_jxyl001__data_res__free_unpacked
                     (MtJxyl001__DataRes *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &mt_jxyl001__data_res__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor mt_jxyl001__temp__field_descriptors[4] =
{
  {
    "addr",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(MtJxyl001__Temp, addr),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "temp",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtJxyl001__Temp, temp),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "hum",
    3,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtJxyl001__Temp, hum),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "battemp",
    4,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtJxyl001__Temp, battemp),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned mt_jxyl001__temp__field_indices_by_name[] = {
  0,   /* field[0] = addr */
  3,   /* field[3] = battemp */
  2,   /* field[2] = hum */
  1,   /* field[1] = temp */
};
static const ProtobufCIntRange mt_jxyl001__temp__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 4 }
};
const ProtobufCMessageDescriptor mt_jxyl001__temp__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "mt_jxyl001.Temp",
  "Temp",
  "MtJxyl001__Temp",
  "mt_jxyl001",
  sizeof(MtJxyl001__Temp),
  4,
  mt_jxyl001__temp__field_descriptors,
  mt_jxyl001__temp__field_indices_by_name,
  1,  mt_jxyl001__temp__number_ranges,
  (ProtobufCMessageInit) mt_jxyl001__temp__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor mt_jxyl001__fog__field_descriptors[3] =
{
  {
    "addr",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(MtJxyl001__Fog, addr),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "frog",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtJxyl001__Fog, frog),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "batfog",
    3,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtJxyl001__Fog, batfog),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned mt_jxyl001__fog__field_indices_by_name[] = {
  0,   /* field[0] = addr */
  2,   /* field[2] = batfog */
  1,   /* field[1] = frog */
};
static const ProtobufCIntRange mt_jxyl001__fog__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 3 }
};
const ProtobufCMessageDescriptor mt_jxyl001__fog__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "mt_jxyl001.Fog",
  "Fog",
  "MtJxyl001__Fog",
  "mt_jxyl001",
  sizeof(MtJxyl001__Fog),
  3,
  mt_jxyl001__fog__field_descriptors,
  mt_jxyl001__fog__field_indices_by_name,
  1,  mt_jxyl001__fog__number_ranges,
  (ProtobufCMessageInit) mt_jxyl001__fog__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor mt_jxyl001__data_res__field_descriptors[3] =
{
  {
    "resCode",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(MtJxyl001__DataRes, rescode),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "temps",
    2,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_MESSAGE,
    offsetof(MtJxyl001__DataRes, n_temps),
    offsetof(MtJxyl001__DataRes, temps),
    &mt_jxyl001__temp__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "fogs",
    3,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_MESSAGE,
    offsetof(MtJxyl001__DataRes, n_fogs),
    offsetof(MtJxyl001__DataRes, fogs),
    &mt_jxyl001__fog__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned mt_jxyl001__data_res__field_indices_by_name[] = {
  2,   /* field[2] = fogs */
  0,   /* field[0] = resCode */
  1,   /* field[1] = temps */
};
static const ProtobufCIntRange mt_jxyl001__data_res__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 3 }
};
const ProtobufCMessageDescriptor mt_jxyl001__data_res__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "mt_jxyl001.DataRes",
  "DataRes",
  "MtJxyl001__DataRes",
  "mt_jxyl001",
  sizeof(MtJxyl001__DataRes),
  3,
  mt_jxyl001__data_res__field_descriptors,
  mt_jxyl001__data_res__field_indices_by_name,
  1,  mt_jxyl001__data_res__number_ranges,
  (ProtobufCMessageInit) mt_jxyl001__data_res__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCMethodDescriptor mt_jxyl001__jxyl001_service__method_descriptors[1] =
{
  { "GetData", &google__protobuf__empty__descriptor, &mt_jxyl001__data_res__descriptor },
};
const unsigned mt_jxyl001__jxyl001_service__method_indices_by_name[] = {
  0         /* GetData */
};
const ProtobufCServiceDescriptor mt_jxyl001__jxyl001_service__descriptor =
{
  PROTOBUF_C__SERVICE_DESCRIPTOR_MAGIC,
  "mt_jxyl001.Jxyl001Service",
  "Jxyl001Service",
  "MtJxyl001__Jxyl001Service",
  "mt_jxyl001",
  1,
  mt_jxyl001__jxyl001_service__method_descriptors,
  mt_jxyl001__jxyl001_service__method_indices_by_name
};
void mt_jxyl001__jxyl001_service__get_data(ProtobufCService *service,
                                           const Google__Protobuf__Empty *input,
                                           MtJxyl001__DataRes_Closure closure,
                                           void *closure_data)
{
  assert(service->descriptor == &mt_jxyl001__jxyl001_service__descriptor);
  service->invoke(service, 0, (const ProtobufCMessage *) input, (ProtobufCClosure) closure, closure_data);
}
void mt_jxyl001__jxyl001_service__init (MtJxyl001__Jxyl001Service_Service *service,
                                        MtJxyl001__Jxyl001Service_ServiceDestroy destroy)
{
  protobuf_c_service_generated_init (&service->base,
                                     &mt_jxyl001__jxyl001_service__descriptor,
                                     (ProtobufCServiceDestroy) destroy);
}
