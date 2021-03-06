/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: mt_proto_charge001.proto */

#ifndef PROTOBUF_C_mt_5fproto_5fcharge001_2eproto__INCLUDED
#define PROTOBUF_C_mt_5fproto_5fcharge001_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1003000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1003003 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif

#include "google/protobuf/empty.pb-c.h"

typedef struct _MtCharge001__Charge MtCharge001__Charge;
typedef struct _MtCharge001__State1 MtCharge001__State1;
typedef struct _MtCharge001__State2 MtCharge001__State2;
typedef struct _MtCharge001__SetRes MtCharge001__SetRes;
typedef struct _MtCharge001__SetChargeReq MtCharge001__SetChargeReq;
typedef struct _MtCharge001__GetStateReq MtCharge001__GetStateReq;
typedef struct _MtCharge001__GetStatesRes MtCharge001__GetStatesRes;
typedef struct _MtCharge001__GetStateRes MtCharge001__GetStateRes;


/* --- enums --- */


/* --- messages --- */

struct  _MtCharge001__Charge
{
  ProtobufCMessage base;
  /*
   * 充电端口
   */
  int32_t port;
  /*
   * 充电时间
   */
  int32_t time;
  /*
   * 充电金额 (?设备自己统计用?)
   */
  int32_t money;
};
#define MT_CHARGE001__CHARGE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_charge001__charge__descriptor) \
    , 0, 0, 0 }


struct  _MtCharge001__State1
{
  ProtobufCMessage base;
  /*
   * 端口编号
   */
  int32_t port;
  /*
   * 端口状态
   */
  int32_t state;
};
#define MT_CHARGE001__STATE1__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_charge001__state1__descriptor) \
    , 0, 0 }


struct  _MtCharge001__State2
{
  ProtobufCMessage base;
  /*
   * 端口编号
   */
  int32_t port;
  /*
   * 端口状态
   */
  int32_t state;
  /*
   * 剩余充电时间
   */
  int32_t lefttime;
  /*
   * 充电功率
   */
  int32_t power;
};
#define MT_CHARGE001__STATE2__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_charge001__state2__descriptor) \
    , 0, 0, 0, 0 }


struct  _MtCharge001__SetRes
{
  ProtobufCMessage base;
  int32_t code;
  char *content;
};
#define MT_CHARGE001__SET_RES__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_charge001__set_res__descriptor) \
    , 0, (char *)protobuf_c_empty_string }


struct  _MtCharge001__SetChargeReq
{
  ProtobufCMessage base;
  MtCharge001__Charge *charge;
};
#define MT_CHARGE001__SET_CHARGE_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_charge001__set_charge_req__descriptor) \
    , NULL }


struct  _MtCharge001__GetStateReq
{
  ProtobufCMessage base;
  /*
   * 端口编号
   */
  int32_t port;
};
#define MT_CHARGE001__GET_STATE_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_charge001__get_state_req__descriptor) \
    , 0 }


struct  _MtCharge001__GetStatesRes
{
  ProtobufCMessage base;
  int32_t code;
  /*
   *端口总数
   */
  int32_t portnum;
  /*
   * 端口状态
   */
  size_t n_states;
  MtCharge001__State2 **states;
};
#define MT_CHARGE001__GET_STATES_RES__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_charge001__get_states_res__descriptor) \
    , 0, 0, 0,NULL }


struct  _MtCharge001__GetStateRes
{
  ProtobufCMessage base;
  int32_t code;
  /*
   * 端口状态
   */
  MtCharge001__State2 *state;
};
#define MT_CHARGE001__GET_STATE_RES__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_charge001__get_state_res__descriptor) \
    , 0, NULL }


/* MtCharge001__Charge methods */
void   mt_charge001__charge__init
                     (MtCharge001__Charge         *message);
size_t mt_charge001__charge__get_packed_size
                     (const MtCharge001__Charge   *message);
size_t mt_charge001__charge__pack
                     (const MtCharge001__Charge   *message,
                      uint8_t             *out);
size_t mt_charge001__charge__pack_to_buffer
                     (const MtCharge001__Charge   *message,
                      ProtobufCBuffer     *buffer);
MtCharge001__Charge *
       mt_charge001__charge__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_charge001__charge__free_unpacked
                     (MtCharge001__Charge *message,
                      ProtobufCAllocator *allocator);
/* MtCharge001__State1 methods */
void   mt_charge001__state1__init
                     (MtCharge001__State1         *message);
size_t mt_charge001__state1__get_packed_size
                     (const MtCharge001__State1   *message);
size_t mt_charge001__state1__pack
                     (const MtCharge001__State1   *message,
                      uint8_t             *out);
size_t mt_charge001__state1__pack_to_buffer
                     (const MtCharge001__State1   *message,
                      ProtobufCBuffer     *buffer);
MtCharge001__State1 *
       mt_charge001__state1__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_charge001__state1__free_unpacked
                     (MtCharge001__State1 *message,
                      ProtobufCAllocator *allocator);
/* MtCharge001__State2 methods */
void   mt_charge001__state2__init
                     (MtCharge001__State2         *message);
size_t mt_charge001__state2__get_packed_size
                     (const MtCharge001__State2   *message);
size_t mt_charge001__state2__pack
                     (const MtCharge001__State2   *message,
                      uint8_t             *out);
size_t mt_charge001__state2__pack_to_buffer
                     (const MtCharge001__State2   *message,
                      ProtobufCBuffer     *buffer);
MtCharge001__State2 *
       mt_charge001__state2__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_charge001__state2__free_unpacked
                     (MtCharge001__State2 *message,
                      ProtobufCAllocator *allocator);
/* MtCharge001__SetRes methods */
void   mt_charge001__set_res__init
                     (MtCharge001__SetRes         *message);
size_t mt_charge001__set_res__get_packed_size
                     (const MtCharge001__SetRes   *message);
size_t mt_charge001__set_res__pack
                     (const MtCharge001__SetRes   *message,
                      uint8_t             *out);
size_t mt_charge001__set_res__pack_to_buffer
                     (const MtCharge001__SetRes   *message,
                      ProtobufCBuffer     *buffer);
MtCharge001__SetRes *
       mt_charge001__set_res__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_charge001__set_res__free_unpacked
                     (MtCharge001__SetRes *message,
                      ProtobufCAllocator *allocator);
/* MtCharge001__SetChargeReq methods */
void   mt_charge001__set_charge_req__init
                     (MtCharge001__SetChargeReq         *message);
size_t mt_charge001__set_charge_req__get_packed_size
                     (const MtCharge001__SetChargeReq   *message);
size_t mt_charge001__set_charge_req__pack
                     (const MtCharge001__SetChargeReq   *message,
                      uint8_t             *out);
size_t mt_charge001__set_charge_req__pack_to_buffer
                     (const MtCharge001__SetChargeReq   *message,
                      ProtobufCBuffer     *buffer);
MtCharge001__SetChargeReq *
       mt_charge001__set_charge_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_charge001__set_charge_req__free_unpacked
                     (MtCharge001__SetChargeReq *message,
                      ProtobufCAllocator *allocator);
/* MtCharge001__GetStateReq methods */
void   mt_charge001__get_state_req__init
                     (MtCharge001__GetStateReq         *message);
size_t mt_charge001__get_state_req__get_packed_size
                     (const MtCharge001__GetStateReq   *message);
size_t mt_charge001__get_state_req__pack
                     (const MtCharge001__GetStateReq   *message,
                      uint8_t             *out);
size_t mt_charge001__get_state_req__pack_to_buffer
                     (const MtCharge001__GetStateReq   *message,
                      ProtobufCBuffer     *buffer);
MtCharge001__GetStateReq *
       mt_charge001__get_state_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_charge001__get_state_req__free_unpacked
                     (MtCharge001__GetStateReq *message,
                      ProtobufCAllocator *allocator);
/* MtCharge001__GetStatesRes methods */
void   mt_charge001__get_states_res__init
                     (MtCharge001__GetStatesRes         *message);
size_t mt_charge001__get_states_res__get_packed_size
                     (const MtCharge001__GetStatesRes   *message);
size_t mt_charge001__get_states_res__pack
                     (const MtCharge001__GetStatesRes   *message,
                      uint8_t             *out);
size_t mt_charge001__get_states_res__pack_to_buffer
                     (const MtCharge001__GetStatesRes   *message,
                      ProtobufCBuffer     *buffer);
MtCharge001__GetStatesRes *
       mt_charge001__get_states_res__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_charge001__get_states_res__free_unpacked
                     (MtCharge001__GetStatesRes *message,
                      ProtobufCAllocator *allocator);
/* MtCharge001__GetStateRes methods */
void   mt_charge001__get_state_res__init
                     (MtCharge001__GetStateRes         *message);
size_t mt_charge001__get_state_res__get_packed_size
                     (const MtCharge001__GetStateRes   *message);
size_t mt_charge001__get_state_res__pack
                     (const MtCharge001__GetStateRes   *message,
                      uint8_t             *out);
size_t mt_charge001__get_state_res__pack_to_buffer
                     (const MtCharge001__GetStateRes   *message,
                      ProtobufCBuffer     *buffer);
MtCharge001__GetStateRes *
       mt_charge001__get_state_res__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_charge001__get_state_res__free_unpacked
                     (MtCharge001__GetStateRes *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*MtCharge001__Charge_Closure)
                 (const MtCharge001__Charge *message,
                  void *closure_data);
typedef void (*MtCharge001__State1_Closure)
                 (const MtCharge001__State1 *message,
                  void *closure_data);
typedef void (*MtCharge001__State2_Closure)
                 (const MtCharge001__State2 *message,
                  void *closure_data);
typedef void (*MtCharge001__SetRes_Closure)
                 (const MtCharge001__SetRes *message,
                  void *closure_data);
typedef void (*MtCharge001__SetChargeReq_Closure)
                 (const MtCharge001__SetChargeReq *message,
                  void *closure_data);
typedef void (*MtCharge001__GetStateReq_Closure)
                 (const MtCharge001__GetStateReq *message,
                  void *closure_data);
typedef void (*MtCharge001__GetStatesRes_Closure)
                 (const MtCharge001__GetStatesRes *message,
                  void *closure_data);
typedef void (*MtCharge001__GetStateRes_Closure)
                 (const MtCharge001__GetStateRes *message,
                  void *closure_data);

/* --- services --- */

typedef struct _MtCharge001__Service_Service MtCharge001__Service_Service;
struct _MtCharge001__Service_Service
{
  ProtobufCService base;
  void (*set_charge)(MtCharge001__Service_Service *service,
                     const MtCharge001__SetChargeReq *input,
                     MtCharge001__SetRes_Closure closure,
                     void *closure_data);
  void (*get_states)(MtCharge001__Service_Service *service,
                     const Google__Protobuf__Empty *input,
                     MtCharge001__GetStatesRes_Closure closure,
                     void *closure_data);
  void (*get_state)(MtCharge001__Service_Service *service,
                    const MtCharge001__GetStateReq *input,
                    MtCharge001__GetStateRes_Closure closure,
                    void *closure_data);
};
typedef void (*MtCharge001__Service_ServiceDestroy)(MtCharge001__Service_Service *);
void mt_charge001__service__init (MtCharge001__Service_Service *service,
                                  MtCharge001__Service_ServiceDestroy destroy);
#define MT_CHARGE001__SERVICE__BASE_INIT \
    { &mt_charge001__service__descriptor, protobuf_c_service_invoke_internal, NULL }
#define MT_CHARGE001__SERVICE__INIT(function_prefix__) \
    { MT_CHARGE001__SERVICE__BASE_INIT,\
      function_prefix__ ## set_charge,\
      function_prefix__ ## get_states,\
      function_prefix__ ## get_state  }
void mt_charge001__service__set_charge(ProtobufCService *service,
                                       const MtCharge001__SetChargeReq *input,
                                       MtCharge001__SetRes_Closure closure,
                                       void *closure_data);
void mt_charge001__service__get_states(ProtobufCService *service,
                                       const Google__Protobuf__Empty *input,
                                       MtCharge001__GetStatesRes_Closure closure,
                                       void *closure_data);
void mt_charge001__service__get_state(ProtobufCService *service,
                                      const MtCharge001__GetStateReq *input,
                                      MtCharge001__GetStateRes_Closure closure,
                                      void *closure_data);

/* --- descriptors --- */

extern const ProtobufCMessageDescriptor mt_charge001__charge__descriptor;
extern const ProtobufCMessageDescriptor mt_charge001__state1__descriptor;
extern const ProtobufCMessageDescriptor mt_charge001__state2__descriptor;
extern const ProtobufCMessageDescriptor mt_charge001__set_res__descriptor;
extern const ProtobufCMessageDescriptor mt_charge001__set_charge_req__descriptor;
extern const ProtobufCMessageDescriptor mt_charge001__get_state_req__descriptor;
extern const ProtobufCMessageDescriptor mt_charge001__get_states_res__descriptor;
extern const ProtobufCMessageDescriptor mt_charge001__get_state_res__descriptor;
extern const ProtobufCServiceDescriptor mt_charge001__service__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_mt_5fproto_5fcharge001_2eproto__INCLUDED */
