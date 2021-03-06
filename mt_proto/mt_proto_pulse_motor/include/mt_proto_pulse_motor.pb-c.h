/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: mt_proto_pulse_motor.proto */

#ifndef PROTOBUF_C_mt_5fproto_5fpulse_5fmotor_2eproto__INCLUDED
#define PROTOBUF_C_mt_5fproto_5fpulse_5fmotor_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1003000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1003003 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif

#include "google/protobuf/empty.pb-c.h"

typedef struct _MtPulseMotor__SetStateReq MtPulseMotor__SetStateReq;
typedef struct _MtPulseMotor__SetStateRes MtPulseMotor__SetStateRes;
typedef struct _MtPulseMotor__GetStateRes MtPulseMotor__GetStateRes;
typedef struct _MtPulseMotor__SetStateWithDelayReq MtPulseMotor__SetStateWithDelayReq;
typedef struct _MtPulseMotor__SetStateWithDelayRes MtPulseMotor__SetStateWithDelayRes;


/* --- enums --- */


/* --- messages --- */

struct  _MtPulseMotor__SetStateReq
{
  ProtobufCMessage base;
  protobuf_c_boolean state;
};
#define MT_PULSE_MOTOR__SET_STATE_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_pulse_motor__set_state_req__descriptor) \
    , 0 }


struct  _MtPulseMotor__SetStateRes
{
  ProtobufCMessage base;
  int32_t code;
};
#define MT_PULSE_MOTOR__SET_STATE_RES__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_pulse_motor__set_state_res__descriptor) \
    , 0 }


struct  _MtPulseMotor__GetStateRes
{
  ProtobufCMessage base;
  int32_t code;
  /*
   * true: open  false: close
   */
  protobuf_c_boolean state;
  /*
   * unit: s
   */
  float lefttime;
};
#define MT_PULSE_MOTOR__GET_STATE_RES__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_pulse_motor__get_state_res__descriptor) \
    , 0, 0, 0 }


struct  _MtPulseMotor__SetStateWithDelayReq
{
  ProtobufCMessage base;
  /*
   * true: open  false: close
   */
  protobuf_c_boolean state;
  /*
   * uint: s min: 0.01s
   */
  float time;
};
#define MT_PULSE_MOTOR__SET_STATE_WITH_DELAY_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_pulse_motor__set_state_with_delay_req__descriptor) \
    , 0, 0 }


struct  _MtPulseMotor__SetStateWithDelayRes
{
  ProtobufCMessage base;
  int32_t code;
};
#define MT_PULSE_MOTOR__SET_STATE_WITH_DELAY_RES__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_pulse_motor__set_state_with_delay_res__descriptor) \
    , 0 }


/* MtPulseMotor__SetStateReq methods */
void   mt_pulse_motor__set_state_req__init
                     (MtPulseMotor__SetStateReq         *message);
size_t mt_pulse_motor__set_state_req__get_packed_size
                     (const MtPulseMotor__SetStateReq   *message);
size_t mt_pulse_motor__set_state_req__pack
                     (const MtPulseMotor__SetStateReq   *message,
                      uint8_t             *out);
size_t mt_pulse_motor__set_state_req__pack_to_buffer
                     (const MtPulseMotor__SetStateReq   *message,
                      ProtobufCBuffer     *buffer);
MtPulseMotor__SetStateReq *
       mt_pulse_motor__set_state_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_pulse_motor__set_state_req__free_unpacked
                     (MtPulseMotor__SetStateReq *message,
                      ProtobufCAllocator *allocator);
/* MtPulseMotor__SetStateRes methods */
void   mt_pulse_motor__set_state_res__init
                     (MtPulseMotor__SetStateRes         *message);
size_t mt_pulse_motor__set_state_res__get_packed_size
                     (const MtPulseMotor__SetStateRes   *message);
size_t mt_pulse_motor__set_state_res__pack
                     (const MtPulseMotor__SetStateRes   *message,
                      uint8_t             *out);
size_t mt_pulse_motor__set_state_res__pack_to_buffer
                     (const MtPulseMotor__SetStateRes   *message,
                      ProtobufCBuffer     *buffer);
MtPulseMotor__SetStateRes *
       mt_pulse_motor__set_state_res__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_pulse_motor__set_state_res__free_unpacked
                     (MtPulseMotor__SetStateRes *message,
                      ProtobufCAllocator *allocator);
/* MtPulseMotor__GetStateRes methods */
void   mt_pulse_motor__get_state_res__init
                     (MtPulseMotor__GetStateRes         *message);
size_t mt_pulse_motor__get_state_res__get_packed_size
                     (const MtPulseMotor__GetStateRes   *message);
size_t mt_pulse_motor__get_state_res__pack
                     (const MtPulseMotor__GetStateRes   *message,
                      uint8_t             *out);
size_t mt_pulse_motor__get_state_res__pack_to_buffer
                     (const MtPulseMotor__GetStateRes   *message,
                      ProtobufCBuffer     *buffer);
MtPulseMotor__GetStateRes *
       mt_pulse_motor__get_state_res__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_pulse_motor__get_state_res__free_unpacked
                     (MtPulseMotor__GetStateRes *message,
                      ProtobufCAllocator *allocator);
/* MtPulseMotor__SetStateWithDelayReq methods */
void   mt_pulse_motor__set_state_with_delay_req__init
                     (MtPulseMotor__SetStateWithDelayReq         *message);
size_t mt_pulse_motor__set_state_with_delay_req__get_packed_size
                     (const MtPulseMotor__SetStateWithDelayReq   *message);
size_t mt_pulse_motor__set_state_with_delay_req__pack
                     (const MtPulseMotor__SetStateWithDelayReq   *message,
                      uint8_t             *out);
size_t mt_pulse_motor__set_state_with_delay_req__pack_to_buffer
                     (const MtPulseMotor__SetStateWithDelayReq   *message,
                      ProtobufCBuffer     *buffer);
MtPulseMotor__SetStateWithDelayReq *
       mt_pulse_motor__set_state_with_delay_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_pulse_motor__set_state_with_delay_req__free_unpacked
                     (MtPulseMotor__SetStateWithDelayReq *message,
                      ProtobufCAllocator *allocator);
/* MtPulseMotor__SetStateWithDelayRes methods */
void   mt_pulse_motor__set_state_with_delay_res__init
                     (MtPulseMotor__SetStateWithDelayRes         *message);
size_t mt_pulse_motor__set_state_with_delay_res__get_packed_size
                     (const MtPulseMotor__SetStateWithDelayRes   *message);
size_t mt_pulse_motor__set_state_with_delay_res__pack
                     (const MtPulseMotor__SetStateWithDelayRes   *message,
                      uint8_t             *out);
size_t mt_pulse_motor__set_state_with_delay_res__pack_to_buffer
                     (const MtPulseMotor__SetStateWithDelayRes   *message,
                      ProtobufCBuffer     *buffer);
MtPulseMotor__SetStateWithDelayRes *
       mt_pulse_motor__set_state_with_delay_res__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_pulse_motor__set_state_with_delay_res__free_unpacked
                     (MtPulseMotor__SetStateWithDelayRes *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*MtPulseMotor__SetStateReq_Closure)
                 (const MtPulseMotor__SetStateReq *message,
                  void *closure_data);
typedef void (*MtPulseMotor__SetStateRes_Closure)
                 (const MtPulseMotor__SetStateRes *message,
                  void *closure_data);
typedef void (*MtPulseMotor__GetStateRes_Closure)
                 (const MtPulseMotor__GetStateRes *message,
                  void *closure_data);
typedef void (*MtPulseMotor__SetStateWithDelayReq_Closure)
                 (const MtPulseMotor__SetStateWithDelayReq *message,
                  void *closure_data);
typedef void (*MtPulseMotor__SetStateWithDelayRes_Closure)
                 (const MtPulseMotor__SetStateWithDelayRes *message,
                  void *closure_data);

/* --- services --- */

typedef struct _MtPulseMotor__PulseMotorService_Service MtPulseMotor__PulseMotorService_Service;
struct _MtPulseMotor__PulseMotorService_Service
{
  ProtobufCService base;
  void (*set_state)(MtPulseMotor__PulseMotorService_Service *service,
                    const MtPulseMotor__SetStateReq *input,
                    MtPulseMotor__SetStateRes_Closure closure,
                    void *closure_data);
  void (*get_state)(MtPulseMotor__PulseMotorService_Service *service,
                    const Google__Protobuf__Empty *input,
                    MtPulseMotor__GetStateRes_Closure closure,
                    void *closure_data);
  void (*set_state_with_delay)(MtPulseMotor__PulseMotorService_Service *service,
                               const MtPulseMotor__SetStateWithDelayReq *input,
                               MtPulseMotor__SetStateWithDelayRes_Closure closure,
                               void *closure_data);
};
typedef void (*MtPulseMotor__PulseMotorService_ServiceDestroy)(MtPulseMotor__PulseMotorService_Service *);
void mt_pulse_motor__pulse_motor_service__init (MtPulseMotor__PulseMotorService_Service *service,
                                                MtPulseMotor__PulseMotorService_ServiceDestroy destroy);
#define MT_PULSE_MOTOR__PULSE_MOTOR_SERVICE__BASE_INIT \
    { &mt_pulse_motor__pulse_motor_service__descriptor, protobuf_c_service_invoke_internal, NULL }
#define MT_PULSE_MOTOR__PULSE_MOTOR_SERVICE__INIT(function_prefix__) \
    { MT_PULSE_MOTOR__PULSE_MOTOR_SERVICE__BASE_INIT,\
      function_prefix__ ## set_state,\
      function_prefix__ ## get_state,\
      function_prefix__ ## set_state_with_delay  }
void mt_pulse_motor__pulse_motor_service__set_state(ProtobufCService *service,
                                                    const MtPulseMotor__SetStateReq *input,
                                                    MtPulseMotor__SetStateRes_Closure closure,
                                                    void *closure_data);
void mt_pulse_motor__pulse_motor_service__get_state(ProtobufCService *service,
                                                    const Google__Protobuf__Empty *input,
                                                    MtPulseMotor__GetStateRes_Closure closure,
                                                    void *closure_data);
void mt_pulse_motor__pulse_motor_service__set_state_with_delay(ProtobufCService *service,
                                                               const MtPulseMotor__SetStateWithDelayReq *input,
                                                               MtPulseMotor__SetStateWithDelayRes_Closure closure,
                                                               void *closure_data);

/* --- descriptors --- */

extern const ProtobufCMessageDescriptor mt_pulse_motor__set_state_req__descriptor;
extern const ProtobufCMessageDescriptor mt_pulse_motor__set_state_res__descriptor;
extern const ProtobufCMessageDescriptor mt_pulse_motor__get_state_res__descriptor;
extern const ProtobufCMessageDescriptor mt_pulse_motor__set_state_with_delay_req__descriptor;
extern const ProtobufCMessageDescriptor mt_pulse_motor__set_state_with_delay_res__descriptor;
extern const ProtobufCServiceDescriptor mt_pulse_motor__pulse_motor_service__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_mt_5fproto_5fpulse_5fmotor_2eproto__INCLUDED */
