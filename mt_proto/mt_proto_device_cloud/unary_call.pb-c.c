/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: unary_call.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "unary_call.pb-c.h"
void   ai__metathings__component__unary_call_request__init
                     (Ai__Metathings__Component__UnaryCallRequest         *message)
{
  static const Ai__Metathings__Component__UnaryCallRequest init_value = AI__METATHINGS__COMPONENT__UNARY_CALL_REQUEST__INIT;
  *message = init_value;
}
size_t ai__metathings__component__unary_call_request__get_packed_size
                     (const Ai__Metathings__Component__UnaryCallRequest *message)
{
  assert(message->base.descriptor == &ai__metathings__component__unary_call_request__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t ai__metathings__component__unary_call_request__pack
                     (const Ai__Metathings__Component__UnaryCallRequest *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &ai__metathings__component__unary_call_request__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t ai__metathings__component__unary_call_request__pack_to_buffer
                     (const Ai__Metathings__Component__UnaryCallRequest *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &ai__metathings__component__unary_call_request__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Ai__Metathings__Component__UnaryCallRequest *
       ai__metathings__component__unary_call_request__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Ai__Metathings__Component__UnaryCallRequest *)
     protobuf_c_message_unpack (&ai__metathings__component__unary_call_request__descriptor,
                                allocator, len, data);
}
void   ai__metathings__component__unary_call_request__free_unpacked
                     (Ai__Metathings__Component__UnaryCallRequest *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &ai__metathings__component__unary_call_request__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   ai__metathings__component__unary_call_response__init
                     (Ai__Metathings__Component__UnaryCallResponse         *message)
{
  static const Ai__Metathings__Component__UnaryCallResponse init_value = AI__METATHINGS__COMPONENT__UNARY_CALL_RESPONSE__INIT;
  *message = init_value;
}
size_t ai__metathings__component__unary_call_response__get_packed_size
                     (const Ai__Metathings__Component__UnaryCallResponse *message)
{
  assert(message->base.descriptor == &ai__metathings__component__unary_call_response__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t ai__metathings__component__unary_call_response__pack
                     (const Ai__Metathings__Component__UnaryCallResponse *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &ai__metathings__component__unary_call_response__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t ai__metathings__component__unary_call_response__pack_to_buffer
                     (const Ai__Metathings__Component__UnaryCallResponse *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &ai__metathings__component__unary_call_response__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Ai__Metathings__Component__UnaryCallResponse *
       ai__metathings__component__unary_call_response__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Ai__Metathings__Component__UnaryCallResponse *)
     protobuf_c_message_unpack (&ai__metathings__component__unary_call_response__descriptor,
                                allocator, len, data);
}
void   ai__metathings__component__unary_call_response__free_unpacked
                     (Ai__Metathings__Component__UnaryCallResponse *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &ai__metathings__component__unary_call_response__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor ai__metathings__component__unary_call_request__field_descriptors[2] =
{
  {
    "method",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(Ai__Metathings__Component__UnaryCallRequest, method),
    &google__protobuf__string_value__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "value",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(Ai__Metathings__Component__UnaryCallRequest, value),
    &google__protobuf__any__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned ai__metathings__component__unary_call_request__field_indices_by_name[] = {
  0,   /* field[0] = method */
  1,   /* field[1] = value */
};
static const ProtobufCIntRange ai__metathings__component__unary_call_request__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 2 }
};
const ProtobufCMessageDescriptor ai__metathings__component__unary_call_request__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "ai.metathings.component.UnaryCallRequest",
  "UnaryCallRequest",
  "Ai__Metathings__Component__UnaryCallRequest",
  "ai.metathings.component",
  sizeof(Ai__Metathings__Component__UnaryCallRequest),
  2,
  ai__metathings__component__unary_call_request__field_descriptors,
  ai__metathings__component__unary_call_request__field_indices_by_name,
  1,  ai__metathings__component__unary_call_request__number_ranges,
  (ProtobufCMessageInit) ai__metathings__component__unary_call_request__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor ai__metathings__component__unary_call_response__field_descriptors[2] =
{
  {
    "method",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Ai__Metathings__Component__UnaryCallResponse, method),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "value",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(Ai__Metathings__Component__UnaryCallResponse, value),
    &google__protobuf__any__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned ai__metathings__component__unary_call_response__field_indices_by_name[] = {
  0,   /* field[0] = method */
  1,   /* field[1] = value */
};
static const ProtobufCIntRange ai__metathings__component__unary_call_response__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 2 }
};
const ProtobufCMessageDescriptor ai__metathings__component__unary_call_response__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "ai.metathings.component.UnaryCallResponse",
  "UnaryCallResponse",
  "Ai__Metathings__Component__UnaryCallResponse",
  "ai.metathings.component",
  sizeof(Ai__Metathings__Component__UnaryCallResponse),
  2,
  ai__metathings__component__unary_call_response__field_descriptors,
  ai__metathings__component__unary_call_response__field_indices_by_name,
  1,  ai__metathings__component__unary_call_response__number_ranges,
  (ProtobufCMessageInit) ai__metathings__component__unary_call_response__init,
  NULL,NULL,NULL    /* reserved[123] */
};
