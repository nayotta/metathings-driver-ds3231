/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: mt_proto_ktc_n01.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "mt_proto_ktc_n01.pb-c.h"
void   mt_ktc_n01__set_cmd_res__init
                     (MtKtcN01__SetCmdRes         *message)
{
  static const MtKtcN01__SetCmdRes init_value = MT_KTC_N01__SET_CMD_RES__INIT;
  *message = init_value;
}
size_t mt_ktc_n01__set_cmd_res__get_packed_size
                     (const MtKtcN01__SetCmdRes *message)
{
  assert(message->base.descriptor == &mt_ktc_n01__set_cmd_res__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t mt_ktc_n01__set_cmd_res__pack
                     (const MtKtcN01__SetCmdRes *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &mt_ktc_n01__set_cmd_res__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t mt_ktc_n01__set_cmd_res__pack_to_buffer
                     (const MtKtcN01__SetCmdRes *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &mt_ktc_n01__set_cmd_res__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
MtKtcN01__SetCmdRes *
       mt_ktc_n01__set_cmd_res__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (MtKtcN01__SetCmdRes *)
     protobuf_c_message_unpack (&mt_ktc_n01__set_cmd_res__descriptor,
                                allocator, len, data);
}
void   mt_ktc_n01__set_cmd_res__free_unpacked
                     (MtKtcN01__SetCmdRes *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &mt_ktc_n01__set_cmd_res__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   mt_ktc_n01__set_cmd_req__init
                     (MtKtcN01__SetCmdReq         *message)
{
  static const MtKtcN01__SetCmdReq init_value = MT_KTC_N01__SET_CMD_REQ__INIT;
  *message = init_value;
}
size_t mt_ktc_n01__set_cmd_req__get_packed_size
                     (const MtKtcN01__SetCmdReq *message)
{
  assert(message->base.descriptor == &mt_ktc_n01__set_cmd_req__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t mt_ktc_n01__set_cmd_req__pack
                     (const MtKtcN01__SetCmdReq *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &mt_ktc_n01__set_cmd_req__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t mt_ktc_n01__set_cmd_req__pack_to_buffer
                     (const MtKtcN01__SetCmdReq *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &mt_ktc_n01__set_cmd_req__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
MtKtcN01__SetCmdReq *
       mt_ktc_n01__set_cmd_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (MtKtcN01__SetCmdReq *)
     protobuf_c_message_unpack (&mt_ktc_n01__set_cmd_req__descriptor,
                                allocator, len, data);
}
void   mt_ktc_n01__set_cmd_req__free_unpacked
                     (MtKtcN01__SetCmdReq *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &mt_ktc_n01__set_cmd_req__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor mt_ktc_n01__set_cmd_res__field_descriptors[2] =
{
  {
    "code",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(MtKtcN01__SetCmdRes, code),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "content",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(MtKtcN01__SetCmdRes, content),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned mt_ktc_n01__set_cmd_res__field_indices_by_name[] = {
  0,   /* field[0] = code */
  1,   /* field[1] = content */
};
static const ProtobufCIntRange mt_ktc_n01__set_cmd_res__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 2 }
};
const ProtobufCMessageDescriptor mt_ktc_n01__set_cmd_res__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "mt_ktc_n01.SetCmdRes",
  "SetCmdRes",
  "MtKtcN01__SetCmdRes",
  "mt_ktc_n01",
  sizeof(MtKtcN01__SetCmdRes),
  2,
  mt_ktc_n01__set_cmd_res__field_descriptors,
  mt_ktc_n01__set_cmd_res__field_indices_by_name,
  1,  mt_ktc_n01__set_cmd_res__number_ranges,
  (ProtobufCMessageInit) mt_ktc_n01__set_cmd_res__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor mt_ktc_n01__set_cmd_req__field_descriptors[2] =
{
  {
    "port",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(MtKtcN01__SetCmdReq, port),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "cmd",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(MtKtcN01__SetCmdReq, cmd),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned mt_ktc_n01__set_cmd_req__field_indices_by_name[] = {
  1,   /* field[1] = cmd */
  0,   /* field[0] = port */
};
static const ProtobufCIntRange mt_ktc_n01__set_cmd_req__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 2 }
};
const ProtobufCMessageDescriptor mt_ktc_n01__set_cmd_req__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "mt_ktc_n01.SetCmdReq",
  "SetCmdReq",
  "MtKtcN01__SetCmdReq",
  "mt_ktc_n01",
  sizeof(MtKtcN01__SetCmdReq),
  2,
  mt_ktc_n01__set_cmd_req__field_descriptors,
  mt_ktc_n01__set_cmd_req__field_indices_by_name,
  1,  mt_ktc_n01__set_cmd_req__number_ranges,
  (ProtobufCMessageInit) mt_ktc_n01__set_cmd_req__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCMethodDescriptor mt_ktc_n01__service__method_descriptors[1] =
{
  { "SetCmd", &mt_ktc_n01__set_cmd_req__descriptor, &mt_ktc_n01__set_cmd_res__descriptor },
};
const unsigned mt_ktc_n01__service__method_indices_by_name[] = {
  0         /* SetCmd */
};
const ProtobufCServiceDescriptor mt_ktc_n01__service__descriptor =
{
  PROTOBUF_C__SERVICE_DESCRIPTOR_MAGIC,
  "mt_ktc_n01.Service",
  "Service",
  "MtKtcN01__Service",
  "mt_ktc_n01",
  1,
  mt_ktc_n01__service__method_descriptors,
  mt_ktc_n01__service__method_indices_by_name
};
void mt_ktc_n01__service__set_cmd(ProtobufCService *service,
                                  const MtKtcN01__SetCmdReq *input,
                                  MtKtcN01__SetCmdRes_Closure closure,
                                  void *closure_data)
{
  assert(service->descriptor == &mt_ktc_n01__service__descriptor);
  service->invoke(service, 0, (const ProtobufCMessage *) input, (ProtobufCClosure) closure, closure_data);
}
void mt_ktc_n01__service__init (MtKtcN01__Service_Service *service,
                                MtKtcN01__Service_ServiceDestroy destroy)
{
  protobuf_c_service_generated_init (&service->base,
                                     &mt_ktc_n01__service__descriptor,
                                     (ProtobufCServiceDestroy) destroy);
}
