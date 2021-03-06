/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: push_frame_to_flow.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "push_frame_to_flow.pb-c.h"
void   push_frame_to_flow_request__config__init
                     (PushFrameToFlowRequest__Config         *message)
{
  static const PushFrameToFlowRequest__Config init_value = PUSH_FRAME_TO_FLOW_REQUEST__CONFIG__INIT;
  *message = init_value;
}
void   push_frame_to_flow_request__ping__init
                     (PushFrameToFlowRequest__Ping         *message)
{
  static const PushFrameToFlowRequest__Ping init_value = PUSH_FRAME_TO_FLOW_REQUEST__PING__INIT;
  *message = init_value;
}
void   push_frame_to_flow_request__init
                     (PushFrameToFlowRequest         *message)
{
  static const PushFrameToFlowRequest init_value = PUSH_FRAME_TO_FLOW_REQUEST__INIT;
  *message = init_value;
}
size_t push_frame_to_flow_request__get_packed_size
                     (const PushFrameToFlowRequest *message)
{
  assert(message->base.descriptor == &push_frame_to_flow_request__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t push_frame_to_flow_request__pack
                     (const PushFrameToFlowRequest *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &push_frame_to_flow_request__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t push_frame_to_flow_request__pack_to_buffer
                     (const PushFrameToFlowRequest *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &push_frame_to_flow_request__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
PushFrameToFlowRequest *
       push_frame_to_flow_request__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (PushFrameToFlowRequest *)
     protobuf_c_message_unpack (&push_frame_to_flow_request__descriptor,
                                allocator, len, data);
}
void   push_frame_to_flow_request__free_unpacked
                     (PushFrameToFlowRequest *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &push_frame_to_flow_request__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   push_frame_to_flow_response__config__init
                     (PushFrameToFlowResponse__Config         *message)
{
  static const PushFrameToFlowResponse__Config init_value = PUSH_FRAME_TO_FLOW_RESPONSE__CONFIG__INIT;
  *message = init_value;
}
void   push_frame_to_flow_response__ack__init
                     (PushFrameToFlowResponse__Ack         *message)
{
  static const PushFrameToFlowResponse__Ack init_value = PUSH_FRAME_TO_FLOW_RESPONSE__ACK__INIT;
  *message = init_value;
}
void   push_frame_to_flow_response__pong__init
                     (PushFrameToFlowResponse__Pong         *message)
{
  static const PushFrameToFlowResponse__Pong init_value = PUSH_FRAME_TO_FLOW_RESPONSE__PONG__INIT;
  *message = init_value;
}
void   push_frame_to_flow_response__init
                     (PushFrameToFlowResponse         *message)
{
  static const PushFrameToFlowResponse init_value = PUSH_FRAME_TO_FLOW_RESPONSE__INIT;
  *message = init_value;
}
size_t push_frame_to_flow_response__get_packed_size
                     (const PushFrameToFlowResponse *message)
{
  assert(message->base.descriptor == &push_frame_to_flow_response__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t push_frame_to_flow_response__pack
                     (const PushFrameToFlowResponse *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &push_frame_to_flow_response__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t push_frame_to_flow_response__pack_to_buffer
                     (const PushFrameToFlowResponse *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &push_frame_to_flow_response__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
PushFrameToFlowResponse *
       push_frame_to_flow_response__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (PushFrameToFlowResponse *)
     protobuf_c_message_unpack (&push_frame_to_flow_response__descriptor,
                                allocator, len, data);
}
void   push_frame_to_flow_response__free_unpacked
                     (PushFrameToFlowResponse *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &push_frame_to_flow_response__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   op_flow__init
                     (OpFlow         *message)
{
  static const OpFlow init_value = OP_FLOW__INIT;
  *message = init_value;
}
size_t op_flow__get_packed_size
                     (const OpFlow *message)
{
  assert(message->base.descriptor == &op_flow__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t op_flow__pack
                     (const OpFlow *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &op_flow__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t op_flow__pack_to_buffer
                     (const OpFlow *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &op_flow__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
OpFlow *
       op_flow__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (OpFlow *)
     protobuf_c_message_unpack (&op_flow__descriptor,
                                allocator, len, data);
}
void   op_flow__free_unpacked
                     (OpFlow *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &op_flow__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   flow__init
                     (Flow         *message)
{
  static const Flow init_value = FLOW__INIT;
  *message = init_value;
}
size_t flow__get_packed_size
                     (const Flow *message)
{
  assert(message->base.descriptor == &flow__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t flow__pack
                     (const Flow *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &flow__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t flow__pack_to_buffer
                     (const Flow *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &flow__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Flow *
       flow__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Flow *)
     protobuf_c_message_unpack (&flow__descriptor,
                                allocator, len, data);
}
void   flow__free_unpacked
                     (Flow *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &flow__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   op_frame__init
                     (OpFrame         *message)
{
  static const OpFrame init_value = OP_FRAME__INIT;
  *message = init_value;
}
size_t op_frame__get_packed_size
                     (const OpFrame *message)
{
  assert(message->base.descriptor == &op_frame__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t op_frame__pack
                     (const OpFrame *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &op_frame__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t op_frame__pack_to_buffer
                     (const OpFrame *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &op_frame__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
OpFrame *
       op_frame__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (OpFrame *)
     protobuf_c_message_unpack (&op_frame__descriptor,
                                allocator, len, data);
}
void   op_frame__free_unpacked
                     (OpFrame *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &op_frame__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   frame__init
                     (Frame         *message)
{
  static const Frame init_value = FRAME__INIT;
  *message = init_value;
}
size_t frame__get_packed_size
                     (const Frame *message)
{
  assert(message->base.descriptor == &frame__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t frame__pack
                     (const Frame *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &frame__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t frame__pack_to_buffer
                     (const Frame *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &frame__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Frame *
       frame__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Frame *)
     protobuf_c_message_unpack (&frame__descriptor,
                                allocator, len, data);
}
void   frame__free_unpacked
                     (Frame *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &frame__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor push_frame_to_flow_request__config__field_descriptors[3] =
{
  {
    "flow",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(PushFrameToFlowRequest__Config, flow),
    &op_flow__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "config_ack",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(PushFrameToFlowRequest__Config, config_ack),
    &google__protobuf__bool_value__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "push_ack",
    3,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(PushFrameToFlowRequest__Config, push_ack),
    &google__protobuf__bool_value__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned push_frame_to_flow_request__config__field_indices_by_name[] = {
  1,   /* field[1] = config_ack */
  0,   /* field[0] = flow */
  2,   /* field[2] = push_ack */
};
static const ProtobufCIntRange push_frame_to_flow_request__config__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 3 }
};
const ProtobufCMessageDescriptor push_frame_to_flow_request__config__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "PushFrameToFlowRequest.Config",
  "Config",
  "PushFrameToFlowRequest__Config",
  "",
  sizeof(PushFrameToFlowRequest__Config),
  3,
  push_frame_to_flow_request__config__field_descriptors,
  push_frame_to_flow_request__config__field_indices_by_name,
  1,  push_frame_to_flow_request__config__number_ranges,
  (ProtobufCMessageInit) push_frame_to_flow_request__config__init,
  NULL,NULL,NULL    /* reserved[123] */
};
#define push_frame_to_flow_request__ping__field_descriptors NULL
#define push_frame_to_flow_request__ping__field_indices_by_name NULL
#define push_frame_to_flow_request__ping__number_ranges NULL
const ProtobufCMessageDescriptor push_frame_to_flow_request__ping__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "PushFrameToFlowRequest.Ping",
  "Ping",
  "PushFrameToFlowRequest__Ping",
  "",
  sizeof(PushFrameToFlowRequest__Ping),
  0,
  push_frame_to_flow_request__ping__field_descriptors,
  push_frame_to_flow_request__ping__field_indices_by_name,
  0,  push_frame_to_flow_request__ping__number_ranges,
  (ProtobufCMessageInit) push_frame_to_flow_request__ping__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor push_frame_to_flow_request__field_descriptors[4] =
{
  {
    "id",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(PushFrameToFlowRequest, id),
    &google__protobuf__string_value__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "config",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    offsetof(PushFrameToFlowRequest, request_case),
    offsetof(PushFrameToFlowRequest, config),
    &push_frame_to_flow_request__config__descriptor,
    NULL,
    0 | PROTOBUF_C_FIELD_FLAG_ONEOF,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "ping",
    3,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    offsetof(PushFrameToFlowRequest, request_case),
    offsetof(PushFrameToFlowRequest, ping),
    &push_frame_to_flow_request__ping__descriptor,
    NULL,
    0 | PROTOBUF_C_FIELD_FLAG_ONEOF,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "frame",
    4,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    offsetof(PushFrameToFlowRequest, request_case),
    offsetof(PushFrameToFlowRequest, frame),
    &op_frame__descriptor,
    NULL,
    0 | PROTOBUF_C_FIELD_FLAG_ONEOF,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned push_frame_to_flow_request__field_indices_by_name[] = {
  1,   /* field[1] = config */
  3,   /* field[3] = frame */
  0,   /* field[0] = id */
  2,   /* field[2] = ping */
};
static const ProtobufCIntRange push_frame_to_flow_request__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 4 }
};
const ProtobufCMessageDescriptor push_frame_to_flow_request__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "PushFrameToFlowRequest",
  "PushFrameToFlowRequest",
  "PushFrameToFlowRequest",
  "",
  sizeof(PushFrameToFlowRequest),
  4,
  push_frame_to_flow_request__field_descriptors,
  push_frame_to_flow_request__field_indices_by_name,
  1,  push_frame_to_flow_request__number_ranges,
  (ProtobufCMessageInit) push_frame_to_flow_request__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor push_frame_to_flow_response__config__field_descriptors[1] =
{
  {
    "session",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(PushFrameToFlowResponse__Config, session),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned push_frame_to_flow_response__config__field_indices_by_name[] = {
  0,   /* field[0] = session */
};
static const ProtobufCIntRange push_frame_to_flow_response__config__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 1 }
};
const ProtobufCMessageDescriptor push_frame_to_flow_response__config__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "PushFrameToFlowResponse.Config",
  "Config",
  "PushFrameToFlowResponse__Config",
  "",
  sizeof(PushFrameToFlowResponse__Config),
  1,
  push_frame_to_flow_response__config__field_descriptors,
  push_frame_to_flow_response__config__field_indices_by_name,
  1,  push_frame_to_flow_response__config__number_ranges,
  (ProtobufCMessageInit) push_frame_to_flow_response__config__init,
  NULL,NULL,NULL    /* reserved[123] */
};
#define push_frame_to_flow_response__ack__field_descriptors NULL
#define push_frame_to_flow_response__ack__field_indices_by_name NULL
#define push_frame_to_flow_response__ack__number_ranges NULL
const ProtobufCMessageDescriptor push_frame_to_flow_response__ack__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "PushFrameToFlowResponse.Ack",
  "Ack",
  "PushFrameToFlowResponse__Ack",
  "",
  sizeof(PushFrameToFlowResponse__Ack),
  0,
  push_frame_to_flow_response__ack__field_descriptors,
  push_frame_to_flow_response__ack__field_indices_by_name,
  0,  push_frame_to_flow_response__ack__number_ranges,
  (ProtobufCMessageInit) push_frame_to_flow_response__ack__init,
  NULL,NULL,NULL    /* reserved[123] */
};
#define push_frame_to_flow_response__pong__field_descriptors NULL
#define push_frame_to_flow_response__pong__field_indices_by_name NULL
#define push_frame_to_flow_response__pong__number_ranges NULL
const ProtobufCMessageDescriptor push_frame_to_flow_response__pong__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "PushFrameToFlowResponse.Pong",
  "Pong",
  "PushFrameToFlowResponse__Pong",
  "",
  sizeof(PushFrameToFlowResponse__Pong),
  0,
  push_frame_to_flow_response__pong__field_descriptors,
  push_frame_to_flow_response__pong__field_indices_by_name,
  0,  push_frame_to_flow_response__pong__number_ranges,
  (ProtobufCMessageInit) push_frame_to_flow_response__pong__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor push_frame_to_flow_response__field_descriptors[4] =
{
  {
    "id",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(PushFrameToFlowResponse, id),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "config",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    offsetof(PushFrameToFlowResponse, response_case),
    offsetof(PushFrameToFlowResponse, config),
    &push_frame_to_flow_response__config__descriptor,
    NULL,
    0 | PROTOBUF_C_FIELD_FLAG_ONEOF,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "pong",
    3,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    offsetof(PushFrameToFlowResponse, response_case),
    offsetof(PushFrameToFlowResponse, pong),
    &push_frame_to_flow_response__pong__descriptor,
    NULL,
    0 | PROTOBUF_C_FIELD_FLAG_ONEOF,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "ack",
    4,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    offsetof(PushFrameToFlowResponse, response_case),
    offsetof(PushFrameToFlowResponse, ack),
    &push_frame_to_flow_response__ack__descriptor,
    NULL,
    0 | PROTOBUF_C_FIELD_FLAG_ONEOF,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned push_frame_to_flow_response__field_indices_by_name[] = {
  3,   /* field[3] = ack */
  1,   /* field[1] = config */
  0,   /* field[0] = id */
  2,   /* field[2] = pong */
};
static const ProtobufCIntRange push_frame_to_flow_response__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 4 }
};
const ProtobufCMessageDescriptor push_frame_to_flow_response__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "PushFrameToFlowResponse",
  "PushFrameToFlowResponse",
  "PushFrameToFlowResponse",
  "",
  sizeof(PushFrameToFlowResponse),
  4,
  push_frame_to_flow_response__field_descriptors,
  push_frame_to_flow_response__field_indices_by_name,
  1,  push_frame_to_flow_response__number_ranges,
  (ProtobufCMessageInit) push_frame_to_flow_response__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor op_flow__field_descriptors[4] =
{
  {
    "id",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(OpFlow, id),
    &google__protobuf__string_value__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "device_id",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(OpFlow, device_id),
    &google__protobuf__string_value__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "name",
    3,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(OpFlow, name),
    &google__protobuf__string_value__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "alias",
    4,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(OpFlow, alias),
    &google__protobuf__string_value__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned op_flow__field_indices_by_name[] = {
  3,   /* field[3] = alias */
  1,   /* field[1] = device_id */
  0,   /* field[0] = id */
  2,   /* field[2] = name */
};
static const ProtobufCIntRange op_flow__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 4 }
};
const ProtobufCMessageDescriptor op_flow__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "OpFlow",
  "OpFlow",
  "OpFlow",
  "",
  sizeof(OpFlow),
  4,
  op_flow__field_descriptors,
  op_flow__field_indices_by_name,
  1,  op_flow__number_ranges,
  (ProtobufCMessageInit) op_flow__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor flow__field_descriptors[4] =
{
  {
    "id",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Flow, id),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "device_id",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Flow, device_id),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "name",
    3,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Flow, name),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "alias",
    4,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Flow, alias),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned flow__field_indices_by_name[] = {
  3,   /* field[3] = alias */
  1,   /* field[1] = device_id */
  0,   /* field[0] = id */
  2,   /* field[2] = name */
};
static const ProtobufCIntRange flow__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 4 }
};
const ProtobufCMessageDescriptor flow__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "Flow",
  "Flow",
  "Flow",
  "",
  sizeof(Flow),
  4,
  flow__field_descriptors,
  flow__field_indices_by_name,
  1,  flow__number_ranges,
  (ProtobufCMessageInit) flow__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor op_frame__field_descriptors[2] =
{
  {
    "ts",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(OpFrame, ts),
    &google__protobuf__timestamp__descriptor,
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
    offsetof(OpFrame, data),
    &google__protobuf__struct__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned op_frame__field_indices_by_name[] = {
  1,   /* field[1] = data */
  0,   /* field[0] = ts */
};
static const ProtobufCIntRange op_frame__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 2 }
};
const ProtobufCMessageDescriptor op_frame__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "OpFrame",
  "OpFrame",
  "OpFrame",
  "",
  sizeof(OpFrame),
  2,
  op_frame__field_descriptors,
  op_frame__field_indices_by_name,
  1,  op_frame__number_ranges,
  (ProtobufCMessageInit) op_frame__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor frame__field_descriptors[2] =
{
  {
    "ts",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(Frame, ts),
    &google__protobuf__timestamp__descriptor,
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
    offsetof(Frame, data),
    &google__protobuf__struct__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned frame__field_indices_by_name[] = {
  1,   /* field[1] = data */
  0,   /* field[0] = ts */
};
static const ProtobufCIntRange frame__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 2 }
};
const ProtobufCMessageDescriptor frame__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "Frame",
  "Frame",
  "Frame",
  "",
  sizeof(Frame),
  2,
  frame__field_descriptors,
  frame__field_indices_by_name,
  1,  frame__number_ranges,
  (ProtobufCMessageInit) frame__init,
  NULL,NULL,NULL    /* reserved[123] */
};
