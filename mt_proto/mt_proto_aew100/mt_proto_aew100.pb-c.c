/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: mt_proto_aew100.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "mt_proto_aew100.pb-c.h"
void   mt_aew100__data__init
                     (MtAew100__Data         *message)
{
  static const MtAew100__Data init_value = MT_AEW100__DATA__INIT;
  *message = init_value;
}
size_t mt_aew100__data__get_packed_size
                     (const MtAew100__Data *message)
{
  assert(message->base.descriptor == &mt_aew100__data__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t mt_aew100__data__pack
                     (const MtAew100__Data *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &mt_aew100__data__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t mt_aew100__data__pack_to_buffer
                     (const MtAew100__Data *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &mt_aew100__data__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
MtAew100__Data *
       mt_aew100__data__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (MtAew100__Data *)
     protobuf_c_message_unpack (&mt_aew100__data__descriptor,
                                allocator, len, data);
}
void   mt_aew100__data__free_unpacked
                     (MtAew100__Data *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &mt_aew100__data__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   mt_aew100__get_data_req__init
                     (MtAew100__GetDataReq         *message)
{
  static const MtAew100__GetDataReq init_value = MT_AEW100__GET_DATA_REQ__INIT;
  *message = init_value;
}
size_t mt_aew100__get_data_req__get_packed_size
                     (const MtAew100__GetDataReq *message)
{
  assert(message->base.descriptor == &mt_aew100__get_data_req__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t mt_aew100__get_data_req__pack
                     (const MtAew100__GetDataReq *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &mt_aew100__get_data_req__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t mt_aew100__get_data_req__pack_to_buffer
                     (const MtAew100__GetDataReq *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &mt_aew100__get_data_req__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
MtAew100__GetDataReq *
       mt_aew100__get_data_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (MtAew100__GetDataReq *)
     protobuf_c_message_unpack (&mt_aew100__get_data_req__descriptor,
                                allocator, len, data);
}
void   mt_aew100__get_data_req__free_unpacked
                     (MtAew100__GetDataReq *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &mt_aew100__get_data_req__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   mt_aew100__get_data_res__init
                     (MtAew100__GetDataRes         *message)
{
  static const MtAew100__GetDataRes init_value = MT_AEW100__GET_DATA_RES__INIT;
  *message = init_value;
}
size_t mt_aew100__get_data_res__get_packed_size
                     (const MtAew100__GetDataRes *message)
{
  assert(message->base.descriptor == &mt_aew100__get_data_res__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t mt_aew100__get_data_res__pack
                     (const MtAew100__GetDataRes *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &mt_aew100__get_data_res__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t mt_aew100__get_data_res__pack_to_buffer
                     (const MtAew100__GetDataRes *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &mt_aew100__get_data_res__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
MtAew100__GetDataRes *
       mt_aew100__get_data_res__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (MtAew100__GetDataRes *)
     protobuf_c_message_unpack (&mt_aew100__get_data_res__descriptor,
                                allocator, len, data);
}
void   mt_aew100__get_data_res__free_unpacked
                     (MtAew100__GetDataRes *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &mt_aew100__get_data_res__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   mt_aew100__current_data__init
                     (MtAew100__CurrentData         *message)
{
  static const MtAew100__CurrentData init_value = MT_AEW100__CURRENT_DATA__INIT;
  *message = init_value;
}
size_t mt_aew100__current_data__get_packed_size
                     (const MtAew100__CurrentData *message)
{
  assert(message->base.descriptor == &mt_aew100__current_data__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t mt_aew100__current_data__pack
                     (const MtAew100__CurrentData *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &mt_aew100__current_data__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t mt_aew100__current_data__pack_to_buffer
                     (const MtAew100__CurrentData *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &mt_aew100__current_data__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
MtAew100__CurrentData *
       mt_aew100__current_data__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (MtAew100__CurrentData *)
     protobuf_c_message_unpack (&mt_aew100__current_data__descriptor,
                                allocator, len, data);
}
void   mt_aew100__current_data__free_unpacked
                     (MtAew100__CurrentData *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &mt_aew100__current_data__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   mt_aew100__get_current_req__init
                     (MtAew100__GetCurrentReq         *message)
{
  static const MtAew100__GetCurrentReq init_value = MT_AEW100__GET_CURRENT_REQ__INIT;
  *message = init_value;
}
size_t mt_aew100__get_current_req__get_packed_size
                     (const MtAew100__GetCurrentReq *message)
{
  assert(message->base.descriptor == &mt_aew100__get_current_req__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t mt_aew100__get_current_req__pack
                     (const MtAew100__GetCurrentReq *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &mt_aew100__get_current_req__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t mt_aew100__get_current_req__pack_to_buffer
                     (const MtAew100__GetCurrentReq *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &mt_aew100__get_current_req__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
MtAew100__GetCurrentReq *
       mt_aew100__get_current_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (MtAew100__GetCurrentReq *)
     protobuf_c_message_unpack (&mt_aew100__get_current_req__descriptor,
                                allocator, len, data);
}
void   mt_aew100__get_current_req__free_unpacked
                     (MtAew100__GetCurrentReq *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &mt_aew100__get_current_req__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   mt_aew100__get_current_res__init
                     (MtAew100__GetCurrentRes         *message)
{
  static const MtAew100__GetCurrentRes init_value = MT_AEW100__GET_CURRENT_RES__INIT;
  *message = init_value;
}
size_t mt_aew100__get_current_res__get_packed_size
                     (const MtAew100__GetCurrentRes *message)
{
  assert(message->base.descriptor == &mt_aew100__get_current_res__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t mt_aew100__get_current_res__pack
                     (const MtAew100__GetCurrentRes *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &mt_aew100__get_current_res__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t mt_aew100__get_current_res__pack_to_buffer
                     (const MtAew100__GetCurrentRes *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &mt_aew100__get_current_res__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
MtAew100__GetCurrentRes *
       mt_aew100__get_current_res__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (MtAew100__GetCurrentRes *)
     protobuf_c_message_unpack (&mt_aew100__get_current_res__descriptor,
                                allocator, len, data);
}
void   mt_aew100__get_current_res__free_unpacked
                     (MtAew100__GetCurrentRes *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &mt_aew100__get_current_res__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor mt_aew100__data__field_descriptors[24] =
{
  {
    "currentA",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, currenta),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "currentB",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, currentb),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "currentC",
    3,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, currentc),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "votageA",
    4,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, votagea),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "votageB",
    5,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, votageb),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "votageC",
    6,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, votagec),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "activePowerA",
    7,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, activepowera),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "activePowerB",
    8,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, activepowerb),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "activePowerC",
    9,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, activepowerc),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "reactivePowerA",
    10,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, reactivepowera),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "reactivePowerB",
    11,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, reactivepowerb),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "reactivePowerC",
    12,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, reactivepowerc),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "powerFactorA",
    13,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, powerfactora),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "powerFactorB",
    14,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, powerfactorb),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "powerFactorC",
    15,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, powerfactorc),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "qualityA",
    16,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, qualitya),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "qualityB",
    17,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, qualityb),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "qualityC",
    18,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, qualityc),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "tempA",
    19,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, tempa),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "tempB",
    20,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, tempb),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "tempC",
    21,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, tempc),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "maxPowerA",
    22,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, maxpowera),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "maxPowerB",
    23,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, maxpowerb),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "maxPowerC",
    24,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__Data, maxpowerc),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned mt_aew100__data__field_indices_by_name[] = {
  6,   /* field[6] = activePowerA */
  7,   /* field[7] = activePowerB */
  8,   /* field[8] = activePowerC */
  0,   /* field[0] = currentA */
  1,   /* field[1] = currentB */
  2,   /* field[2] = currentC */
  21,   /* field[21] = maxPowerA */
  22,   /* field[22] = maxPowerB */
  23,   /* field[23] = maxPowerC */
  12,   /* field[12] = powerFactorA */
  13,   /* field[13] = powerFactorB */
  14,   /* field[14] = powerFactorC */
  15,   /* field[15] = qualityA */
  16,   /* field[16] = qualityB */
  17,   /* field[17] = qualityC */
  9,   /* field[9] = reactivePowerA */
  10,   /* field[10] = reactivePowerB */
  11,   /* field[11] = reactivePowerC */
  18,   /* field[18] = tempA */
  19,   /* field[19] = tempB */
  20,   /* field[20] = tempC */
  3,   /* field[3] = votageA */
  4,   /* field[4] = votageB */
  5,   /* field[5] = votageC */
};
static const ProtobufCIntRange mt_aew100__data__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 24 }
};
const ProtobufCMessageDescriptor mt_aew100__data__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "mt_aew100.Data",
  "Data",
  "MtAew100__Data",
  "mt_aew100",
  sizeof(MtAew100__Data),
  24,
  mt_aew100__data__field_descriptors,
  mt_aew100__data__field_indices_by_name,
  1,  mt_aew100__data__number_ranges,
  (ProtobufCMessageInit) mt_aew100__data__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor mt_aew100__get_data_req__field_descriptors[1] =
{
  {
    "addr",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(MtAew100__GetDataReq, addr),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned mt_aew100__get_data_req__field_indices_by_name[] = {
  0,   /* field[0] = addr */
};
static const ProtobufCIntRange mt_aew100__get_data_req__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 1 }
};
const ProtobufCMessageDescriptor mt_aew100__get_data_req__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "mt_aew100.GetDataReq",
  "GetDataReq",
  "MtAew100__GetDataReq",
  "mt_aew100",
  sizeof(MtAew100__GetDataReq),
  1,
  mt_aew100__get_data_req__field_descriptors,
  mt_aew100__get_data_req__field_indices_by_name,
  1,  mt_aew100__get_data_req__number_ranges,
  (ProtobufCMessageInit) mt_aew100__get_data_req__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor mt_aew100__get_data_res__field_descriptors[2] =
{
  {
    "code",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(MtAew100__GetDataRes, code),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "data",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__GetDataRes, data),
    &mt_aew100__data__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned mt_aew100__get_data_res__field_indices_by_name[] = {
  0,   /* field[0] = code */
  1,   /* field[1] = data */
};
static const ProtobufCIntRange mt_aew100__get_data_res__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 2 }
};
const ProtobufCMessageDescriptor mt_aew100__get_data_res__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "mt_aew100.GetDataRes",
  "GetDataRes",
  "MtAew100__GetDataRes",
  "mt_aew100",
  sizeof(MtAew100__GetDataRes),
  2,
  mt_aew100__get_data_res__field_descriptors,
  mt_aew100__get_data_res__field_indices_by_name,
  1,  mt_aew100__get_data_res__number_ranges,
  (ProtobufCMessageInit) mt_aew100__get_data_res__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor mt_aew100__current_data__field_descriptors[3] =
{
  {
    "currentA",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__CurrentData, currenta),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "currentB",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__CurrentData, currentb),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "currentC",
    3,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__CurrentData, currentc),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned mt_aew100__current_data__field_indices_by_name[] = {
  0,   /* field[0] = currentA */
  1,   /* field[1] = currentB */
  2,   /* field[2] = currentC */
};
static const ProtobufCIntRange mt_aew100__current_data__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 3 }
};
const ProtobufCMessageDescriptor mt_aew100__current_data__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "mt_aew100.CurrentData",
  "CurrentData",
  "MtAew100__CurrentData",
  "mt_aew100",
  sizeof(MtAew100__CurrentData),
  3,
  mt_aew100__current_data__field_descriptors,
  mt_aew100__current_data__field_indices_by_name,
  1,  mt_aew100__current_data__number_ranges,
  (ProtobufCMessageInit) mt_aew100__current_data__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor mt_aew100__get_current_req__field_descriptors[1] =
{
  {
    "addr",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(MtAew100__GetCurrentReq, addr),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned mt_aew100__get_current_req__field_indices_by_name[] = {
  0,   /* field[0] = addr */
};
static const ProtobufCIntRange mt_aew100__get_current_req__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 1 }
};
const ProtobufCMessageDescriptor mt_aew100__get_current_req__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "mt_aew100.GetCurrentReq",
  "GetCurrentReq",
  "MtAew100__GetCurrentReq",
  "mt_aew100",
  sizeof(MtAew100__GetCurrentReq),
  1,
  mt_aew100__get_current_req__field_descriptors,
  mt_aew100__get_current_req__field_indices_by_name,
  1,  mt_aew100__get_current_req__number_ranges,
  (ProtobufCMessageInit) mt_aew100__get_current_req__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor mt_aew100__get_current_res__field_descriptors[2] =
{
  {
    "resCode",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(MtAew100__GetCurrentRes, rescode),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "currentData",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(MtAew100__GetCurrentRes, currentdata),
    &mt_aew100__current_data__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned mt_aew100__get_current_res__field_indices_by_name[] = {
  1,   /* field[1] = currentData */
  0,   /* field[0] = resCode */
};
static const ProtobufCIntRange mt_aew100__get_current_res__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 2 }
};
const ProtobufCMessageDescriptor mt_aew100__get_current_res__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "mt_aew100.GetCurrentRes",
  "GetCurrentRes",
  "MtAew100__GetCurrentRes",
  "mt_aew100",
  sizeof(MtAew100__GetCurrentRes),
  2,
  mt_aew100__get_current_res__field_descriptors,
  mt_aew100__get_current_res__field_indices_by_name,
  1,  mt_aew100__get_current_res__number_ranges,
  (ProtobufCMessageInit) mt_aew100__get_current_res__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCMethodDescriptor mt_aew100__aew100_service__method_descriptors[2] =
{
  { "GetData", &mt_aew100__get_data_req__descriptor, &mt_aew100__get_data_res__descriptor },
  { "GetCurrent", &mt_aew100__get_current_req__descriptor, &mt_aew100__get_current_res__descriptor },
};
const unsigned mt_aew100__aew100_service__method_indices_by_name[] = {
  1,        /* GetCurrent */
  0         /* GetData */
};
const ProtobufCServiceDescriptor mt_aew100__aew100_service__descriptor =
{
  PROTOBUF_C__SERVICE_DESCRIPTOR_MAGIC,
  "mt_aew100.aew100Service",
  "aew100Service",
  "MtAew100__Aew100Service",
  "mt_aew100",
  2,
  mt_aew100__aew100_service__method_descriptors,
  mt_aew100__aew100_service__method_indices_by_name
};
void mt_aew100__aew100_service__get_data(ProtobufCService *service,
                                         const MtAew100__GetDataReq *input,
                                         MtAew100__GetDataRes_Closure closure,
                                         void *closure_data)
{
  assert(service->descriptor == &mt_aew100__aew100_service__descriptor);
  service->invoke(service, 0, (const ProtobufCMessage *) input, (ProtobufCClosure) closure, closure_data);
}
void mt_aew100__aew100_service__get_current(ProtobufCService *service,
                                            const MtAew100__GetCurrentReq *input,
                                            MtAew100__GetCurrentRes_Closure closure,
                                            void *closure_data)
{
  assert(service->descriptor == &mt_aew100__aew100_service__descriptor);
  service->invoke(service, 1, (const ProtobufCMessage *) input, (ProtobufCClosure) closure, closure_data);
}
void mt_aew100__aew100_service__init (MtAew100__Aew100Service_Service *service,
                                      MtAew100__Aew100Service_ServiceDestroy destroy)
{
  protobuf_c_service_generated_init (&service->base,
                                     &mt_aew100__aew100_service__descriptor,
                                     (ProtobufCServiceDestroy) destroy);
}