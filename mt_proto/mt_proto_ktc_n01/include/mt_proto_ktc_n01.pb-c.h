/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: mt_proto_ktc_n01.proto */

#ifndef PROTOBUF_C_mt_5fproto_5fktc_5fn01_2eproto__INCLUDED
#define PROTOBUF_C_mt_5fproto_5fktc_5fn01_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1003000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1003003 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif


typedef struct _MtKtcN01__SetCmdRes MtKtcN01__SetCmdRes;
typedef struct _MtKtcN01__SetCmdReq MtKtcN01__SetCmdReq;


/* --- enums --- */


/* --- messages --- */

struct  _MtKtcN01__SetCmdRes
{
  ProtobufCMessage base;
  int32_t code;
  char *content;
};
#define MT_KTC_N01__SET_CMD_RES__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_ktc_n01__set_cmd_res__descriptor) \
    , 0, (char *)protobuf_c_empty_string }


/*
 * 发送命令
 * 101   开机制冷
 * 102   开机制热
 * 103   关机
 * 01-20 自定义命令
 */
struct  _MtKtcN01__SetCmdReq
{
  ProtobufCMessage base;
  int32_t port;
  int32_t cmd;
};
#define MT_KTC_N01__SET_CMD_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_ktc_n01__set_cmd_req__descriptor) \
    , 0, 0 }


/* MtKtcN01__SetCmdRes methods */
void   mt_ktc_n01__set_cmd_res__init
                     (MtKtcN01__SetCmdRes         *message);
size_t mt_ktc_n01__set_cmd_res__get_packed_size
                     (const MtKtcN01__SetCmdRes   *message);
size_t mt_ktc_n01__set_cmd_res__pack
                     (const MtKtcN01__SetCmdRes   *message,
                      uint8_t             *out);
size_t mt_ktc_n01__set_cmd_res__pack_to_buffer
                     (const MtKtcN01__SetCmdRes   *message,
                      ProtobufCBuffer     *buffer);
MtKtcN01__SetCmdRes *
       mt_ktc_n01__set_cmd_res__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_ktc_n01__set_cmd_res__free_unpacked
                     (MtKtcN01__SetCmdRes *message,
                      ProtobufCAllocator *allocator);
/* MtKtcN01__SetCmdReq methods */
void   mt_ktc_n01__set_cmd_req__init
                     (MtKtcN01__SetCmdReq         *message);
size_t mt_ktc_n01__set_cmd_req__get_packed_size
                     (const MtKtcN01__SetCmdReq   *message);
size_t mt_ktc_n01__set_cmd_req__pack
                     (const MtKtcN01__SetCmdReq   *message,
                      uint8_t             *out);
size_t mt_ktc_n01__set_cmd_req__pack_to_buffer
                     (const MtKtcN01__SetCmdReq   *message,
                      ProtobufCBuffer     *buffer);
MtKtcN01__SetCmdReq *
       mt_ktc_n01__set_cmd_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_ktc_n01__set_cmd_req__free_unpacked
                     (MtKtcN01__SetCmdReq *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*MtKtcN01__SetCmdRes_Closure)
                 (const MtKtcN01__SetCmdRes *message,
                  void *closure_data);
typedef void (*MtKtcN01__SetCmdReq_Closure)
                 (const MtKtcN01__SetCmdReq *message,
                  void *closure_data);

/* --- services --- */

typedef struct _MtKtcN01__Service_Service MtKtcN01__Service_Service;
struct _MtKtcN01__Service_Service
{
  ProtobufCService base;
  void (*set_cmd)(MtKtcN01__Service_Service *service,
                  const MtKtcN01__SetCmdReq *input,
                  MtKtcN01__SetCmdRes_Closure closure,
                  void *closure_data);
};
typedef void (*MtKtcN01__Service_ServiceDestroy)(MtKtcN01__Service_Service *);
void mt_ktc_n01__service__init (MtKtcN01__Service_Service *service,
                                MtKtcN01__Service_ServiceDestroy destroy);
#define MT_KTC_N01__SERVICE__BASE_INIT \
    { &mt_ktc_n01__service__descriptor, protobuf_c_service_invoke_internal, NULL }
#define MT_KTC_N01__SERVICE__INIT(function_prefix__) \
    { MT_KTC_N01__SERVICE__BASE_INIT,\
      function_prefix__ ## set_cmd  }
void mt_ktc_n01__service__set_cmd(ProtobufCService *service,
                                  const MtKtcN01__SetCmdReq *input,
                                  MtKtcN01__SetCmdRes_Closure closure,
                                  void *closure_data);

/* --- descriptors --- */

extern const ProtobufCMessageDescriptor mt_ktc_n01__set_cmd_res__descriptor;
extern const ProtobufCMessageDescriptor mt_ktc_n01__set_cmd_req__descriptor;
extern const ProtobufCServiceDescriptor mt_ktc_n01__service__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_mt_5fproto_5fktc_5fn01_2eproto__INCLUDED */
