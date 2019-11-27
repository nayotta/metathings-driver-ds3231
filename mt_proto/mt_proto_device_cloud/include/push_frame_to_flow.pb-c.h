/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: push_frame_to_flow.proto */

#ifndef PROTOBUF_C_push_5fframe_5fto_5fflow_2eproto__INCLUDED
#define PROTOBUF_C_push_5fframe_5fto_5fflow_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1003000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1003001 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif

#include "google/protobuf/wrappers.pb-c.h"
#include "google/protobuf/timestamp.pb-c.h"
#include "google/protobuf/struct.pb-c.h"

typedef struct _PushFrameToFlowRequest PushFrameToFlowRequest;
typedef struct _PushFrameToFlowRequest__Config PushFrameToFlowRequest__Config;
typedef struct _PushFrameToFlowRequest__Ping PushFrameToFlowRequest__Ping;
typedef struct _PushFrameToFlowResponse PushFrameToFlowResponse;
typedef struct _PushFrameToFlowResponse__Config PushFrameToFlowResponse__Config;
typedef struct _PushFrameToFlowResponse__Ack PushFrameToFlowResponse__Ack;
typedef struct _PushFrameToFlowResponse__Pong PushFrameToFlowResponse__Pong;
typedef struct _OpFlow OpFlow;
typedef struct _Flow Flow;
typedef struct _OpFrame OpFrame;
typedef struct _Frame Frame;


/* --- enums --- */


/* --- messages --- */

struct  _PushFrameToFlowRequest__Config
{
  ProtobufCMessage base;
  OpFlow *flow;
  Google__Protobuf__BoolValue *config_ack;
  Google__Protobuf__BoolValue *push_ack;
};
#define PUSH_FRAME_TO_FLOW_REQUEST__CONFIG__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&push_frame_to_flow_request__config__descriptor) \
    , NULL, NULL, NULL }


struct  _PushFrameToFlowRequest__Ping
{
  ProtobufCMessage base;
};
#define PUSH_FRAME_TO_FLOW_REQUEST__PING__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&push_frame_to_flow_request__ping__descriptor) \
     }


typedef enum {
  PUSH_FRAME_TO_FLOW_REQUEST__REQUEST__NOT_SET = 0,
  PUSH_FRAME_TO_FLOW_REQUEST__REQUEST_CONFIG = 2,
  PUSH_FRAME_TO_FLOW_REQUEST__REQUEST_PING = 3,
  PUSH_FRAME_TO_FLOW_REQUEST__REQUEST_FRAME = 4
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(PUSH_FRAME_TO_FLOW_REQUEST__REQUEST)
} PushFrameToFlowRequest__RequestCase;

struct  _PushFrameToFlowRequest
{
  ProtobufCMessage base;
  Google__Protobuf__StringValue *id;
  PushFrameToFlowRequest__RequestCase request_case;
  union {
    PushFrameToFlowRequest__Config *config;
    PushFrameToFlowRequest__Ping *ping;
    OpFrame *frame;
  };
};
#define PUSH_FRAME_TO_FLOW_REQUEST__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&push_frame_to_flow_request__descriptor) \
    , NULL, PUSH_FRAME_TO_FLOW_REQUEST__REQUEST__NOT_SET, {0} }


struct  _PushFrameToFlowResponse__Config
{
  ProtobufCMessage base;
  char *session;
};
#define PUSH_FRAME_TO_FLOW_RESPONSE__CONFIG__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&push_frame_to_flow_response__config__descriptor) \
    , (char *)protobuf_c_empty_string }


struct  _PushFrameToFlowResponse__Ack
{
  ProtobufCMessage base;
};
#define PUSH_FRAME_TO_FLOW_RESPONSE__ACK__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&push_frame_to_flow_response__ack__descriptor) \
     }


struct  _PushFrameToFlowResponse__Pong
{
  ProtobufCMessage base;
};
#define PUSH_FRAME_TO_FLOW_RESPONSE__PONG__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&push_frame_to_flow_response__pong__descriptor) \
     }


typedef enum {
  PUSH_FRAME_TO_FLOW_RESPONSE__RESPONSE__NOT_SET = 0,
  PUSH_FRAME_TO_FLOW_RESPONSE__RESPONSE_CONFIG = 2,
  PUSH_FRAME_TO_FLOW_RESPONSE__RESPONSE_PONG = 3,
  PUSH_FRAME_TO_FLOW_RESPONSE__RESPONSE_ACK = 4
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(PUSH_FRAME_TO_FLOW_RESPONSE__RESPONSE)
} PushFrameToFlowResponse__ResponseCase;

struct  _PushFrameToFlowResponse
{
  ProtobufCMessage base;
  char *id;
  PushFrameToFlowResponse__ResponseCase response_case;
  union {
    PushFrameToFlowResponse__Config *config;
    PushFrameToFlowResponse__Pong *pong;
    PushFrameToFlowResponse__Ack *ack;
  };
};
#define PUSH_FRAME_TO_FLOW_RESPONSE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&push_frame_to_flow_response__descriptor) \
    , (char *)protobuf_c_empty_string, PUSH_FRAME_TO_FLOW_RESPONSE__RESPONSE__NOT_SET, {0} }


struct  _OpFlow
{
  ProtobufCMessage base;
  Google__Protobuf__StringValue *id;
  Google__Protobuf__StringValue *device_id;
  Google__Protobuf__StringValue *name;
  Google__Protobuf__StringValue *alias;
};
#define OP_FLOW__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&op_flow__descriptor) \
    , NULL, NULL, NULL, NULL }


struct  _Flow
{
  ProtobufCMessage base;
  char *id;
  char *device_id;
  char *name;
  char *alias;
};
#define FLOW__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&flow__descriptor) \
    , (char *)protobuf_c_empty_string, (char *)protobuf_c_empty_string, (char *)protobuf_c_empty_string, (char *)protobuf_c_empty_string }


struct  _OpFrame
{
  ProtobufCMessage base;
  Google__Protobuf__Timestamp *ts;
  Google__Protobuf__Struct *data;
};
#define OP_FRAME__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&op_frame__descriptor) \
    , NULL, NULL }


struct  _Frame
{
  ProtobufCMessage base;
  Google__Protobuf__Timestamp *ts;
  Google__Protobuf__Struct *data;
};
#define FRAME__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&frame__descriptor) \
    , NULL, NULL }


/* PushFrameToFlowRequest__Config methods */
void   push_frame_to_flow_request__config__init
                     (PushFrameToFlowRequest__Config         *message);
/* PushFrameToFlowRequest__Ping methods */
void   push_frame_to_flow_request__ping__init
                     (PushFrameToFlowRequest__Ping         *message);
/* PushFrameToFlowRequest methods */
void   push_frame_to_flow_request__init
                     (PushFrameToFlowRequest         *message);
size_t push_frame_to_flow_request__get_packed_size
                     (const PushFrameToFlowRequest   *message);
size_t push_frame_to_flow_request__pack
                     (const PushFrameToFlowRequest   *message,
                      uint8_t             *out);
size_t push_frame_to_flow_request__pack_to_buffer
                     (const PushFrameToFlowRequest   *message,
                      ProtobufCBuffer     *buffer);
PushFrameToFlowRequest *
       push_frame_to_flow_request__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   push_frame_to_flow_request__free_unpacked
                     (PushFrameToFlowRequest *message,
                      ProtobufCAllocator *allocator);
/* PushFrameToFlowResponse__Config methods */
void   push_frame_to_flow_response__config__init
                     (PushFrameToFlowResponse__Config         *message);
/* PushFrameToFlowResponse__Ack methods */
void   push_frame_to_flow_response__ack__init
                     (PushFrameToFlowResponse__Ack         *message);
/* PushFrameToFlowResponse__Pong methods */
void   push_frame_to_flow_response__pong__init
                     (PushFrameToFlowResponse__Pong         *message);
/* PushFrameToFlowResponse methods */
void   push_frame_to_flow_response__init
                     (PushFrameToFlowResponse         *message);
size_t push_frame_to_flow_response__get_packed_size
                     (const PushFrameToFlowResponse   *message);
size_t push_frame_to_flow_response__pack
                     (const PushFrameToFlowResponse   *message,
                      uint8_t             *out);
size_t push_frame_to_flow_response__pack_to_buffer
                     (const PushFrameToFlowResponse   *message,
                      ProtobufCBuffer     *buffer);
PushFrameToFlowResponse *
       push_frame_to_flow_response__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   push_frame_to_flow_response__free_unpacked
                     (PushFrameToFlowResponse *message,
                      ProtobufCAllocator *allocator);
/* OpFlow methods */
void   op_flow__init
                     (OpFlow         *message);
size_t op_flow__get_packed_size
                     (const OpFlow   *message);
size_t op_flow__pack
                     (const OpFlow   *message,
                      uint8_t             *out);
size_t op_flow__pack_to_buffer
                     (const OpFlow   *message,
                      ProtobufCBuffer     *buffer);
OpFlow *
       op_flow__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   op_flow__free_unpacked
                     (OpFlow *message,
                      ProtobufCAllocator *allocator);
/* Flow methods */
void   flow__init
                     (Flow         *message);
size_t flow__get_packed_size
                     (const Flow   *message);
size_t flow__pack
                     (const Flow   *message,
                      uint8_t             *out);
size_t flow__pack_to_buffer
                     (const Flow   *message,
                      ProtobufCBuffer     *buffer);
Flow *
       flow__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   flow__free_unpacked
                     (Flow *message,
                      ProtobufCAllocator *allocator);
/* OpFrame methods */
void   op_frame__init
                     (OpFrame         *message);
size_t op_frame__get_packed_size
                     (const OpFrame   *message);
size_t op_frame__pack
                     (const OpFrame   *message,
                      uint8_t             *out);
size_t op_frame__pack_to_buffer
                     (const OpFrame   *message,
                      ProtobufCBuffer     *buffer);
OpFrame *
       op_frame__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   op_frame__free_unpacked
                     (OpFrame *message,
                      ProtobufCAllocator *allocator);
/* Frame methods */
void   frame__init
                     (Frame         *message);
size_t frame__get_packed_size
                     (const Frame   *message);
size_t frame__pack
                     (const Frame   *message,
                      uint8_t             *out);
size_t frame__pack_to_buffer
                     (const Frame   *message,
                      ProtobufCBuffer     *buffer);
Frame *
       frame__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   frame__free_unpacked
                     (Frame *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*PushFrameToFlowRequest__Config_Closure)
                 (const PushFrameToFlowRequest__Config *message,
                  void *closure_data);
typedef void (*PushFrameToFlowRequest__Ping_Closure)
                 (const PushFrameToFlowRequest__Ping *message,
                  void *closure_data);
typedef void (*PushFrameToFlowRequest_Closure)
                 (const PushFrameToFlowRequest *message,
                  void *closure_data);
typedef void (*PushFrameToFlowResponse__Config_Closure)
                 (const PushFrameToFlowResponse__Config *message,
                  void *closure_data);
typedef void (*PushFrameToFlowResponse__Ack_Closure)
                 (const PushFrameToFlowResponse__Ack *message,
                  void *closure_data);
typedef void (*PushFrameToFlowResponse__Pong_Closure)
                 (const PushFrameToFlowResponse__Pong *message,
                  void *closure_data);
typedef void (*PushFrameToFlowResponse_Closure)
                 (const PushFrameToFlowResponse *message,
                  void *closure_data);
typedef void (*OpFlow_Closure)
                 (const OpFlow *message,
                  void *closure_data);
typedef void (*Flow_Closure)
                 (const Flow *message,
                  void *closure_data);
typedef void (*OpFrame_Closure)
                 (const OpFrame *message,
                  void *closure_data);
typedef void (*Frame_Closure)
                 (const Frame *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCMessageDescriptor push_frame_to_flow_request__descriptor;
extern const ProtobufCMessageDescriptor push_frame_to_flow_request__config__descriptor;
extern const ProtobufCMessageDescriptor push_frame_to_flow_request__ping__descriptor;
extern const ProtobufCMessageDescriptor push_frame_to_flow_response__descriptor;
extern const ProtobufCMessageDescriptor push_frame_to_flow_response__config__descriptor;
extern const ProtobufCMessageDescriptor push_frame_to_flow_response__ack__descriptor;
extern const ProtobufCMessageDescriptor push_frame_to_flow_response__pong__descriptor;
extern const ProtobufCMessageDescriptor op_flow__descriptor;
extern const ProtobufCMessageDescriptor flow__descriptor;
extern const ProtobufCMessageDescriptor op_frame__descriptor;
extern const ProtobufCMessageDescriptor frame__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_push_5fframe_5fto_5fflow_2eproto__INCLUDED */