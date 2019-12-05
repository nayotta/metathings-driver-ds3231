/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: mt_proto_gzpd800T.proto */

#ifndef PROTOBUF_C_mt_5fproto_5fgzpd800T_2eproto__INCLUDED
#define PROTOBUF_C_mt_5fproto_5fgzpd800T_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1003000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1003001 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif

#include "google/protobuf/empty.pb-c.h"

typedef struct _MtGzpd800T__Data MtGzpd800T__Data;
typedef struct _MtGzpd800T__DataRes MtGzpd800T__DataRes;


/* --- enums --- */


/* --- messages --- */

struct  _MtGzpd800T__Data
{
  ProtobufCMessage base;
  /*
   * 局放传感器编号
   */
  int32_t addr;
  /*
   * 幅值
   */
  uint32_t amp;
  /*
   * 频率
   */
  uint32_t freq;
  /*
   * 总能量
   */
  uint32_t power;
  /*
   * 报警状态
   */
  protobuf_c_boolean warn;
};
#define MT_GZPD800_T__DATA__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_gzpd800_t__data__descriptor) \
    , 0, 0, 0, 0, 0 }


struct  _MtGzpd800T__DataRes
{
  ProtobufCMessage base;
  int32_t rescode;
  size_t n_datas;
  MtGzpd800T__Data **datas;
};
#define MT_GZPD800_T__DATA_RES__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_gzpd800_t__data_res__descriptor) \
    , 0, 0,NULL }


/* MtGzpd800T__Data methods */
void   mt_gzpd800_t__data__init
                     (MtGzpd800T__Data         *message);
size_t mt_gzpd800_t__data__get_packed_size
                     (const MtGzpd800T__Data   *message);
size_t mt_gzpd800_t__data__pack
                     (const MtGzpd800T__Data   *message,
                      uint8_t             *out);
size_t mt_gzpd800_t__data__pack_to_buffer
                     (const MtGzpd800T__Data   *message,
                      ProtobufCBuffer     *buffer);
MtGzpd800T__Data *
       mt_gzpd800_t__data__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_gzpd800_t__data__free_unpacked
                     (MtGzpd800T__Data *message,
                      ProtobufCAllocator *allocator);
/* MtGzpd800T__DataRes methods */
void   mt_gzpd800_t__data_res__init
                     (MtGzpd800T__DataRes         *message);
size_t mt_gzpd800_t__data_res__get_packed_size
                     (const MtGzpd800T__DataRes   *message);
size_t mt_gzpd800_t__data_res__pack
                     (const MtGzpd800T__DataRes   *message,
                      uint8_t             *out);
size_t mt_gzpd800_t__data_res__pack_to_buffer
                     (const MtGzpd800T__DataRes   *message,
                      ProtobufCBuffer     *buffer);
MtGzpd800T__DataRes *
       mt_gzpd800_t__data_res__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_gzpd800_t__data_res__free_unpacked
                     (MtGzpd800T__DataRes *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*MtGzpd800T__Data_Closure)
                 (const MtGzpd800T__Data *message,
                  void *closure_data);
typedef void (*MtGzpd800T__DataRes_Closure)
                 (const MtGzpd800T__DataRes *message,
                  void *closure_data);

/* --- services --- */

typedef struct _MtGzpd800T__Gzpd800TService_Service MtGzpd800T__Gzpd800TService_Service;
struct _MtGzpd800T__Gzpd800TService_Service
{
  ProtobufCService base;
  void (*get_data)(MtGzpd800T__Gzpd800TService_Service *service,
                   const Google__Protobuf__Empty *input,
                   MtGzpd800T__DataRes_Closure closure,
                   void *closure_data);
};
typedef void (*MtGzpd800T__Gzpd800TService_ServiceDestroy)(MtGzpd800T__Gzpd800TService_Service *);
void mt_gzpd800_t__gzpd800_tservice__init (MtGzpd800T__Gzpd800TService_Service *service,
                                           MtGzpd800T__Gzpd800TService_ServiceDestroy destroy);
#define MT_GZPD800_T__GZPD800_TSERVICE__BASE_INIT \
    { &mt_gzpd800_t__gzpd800_tservice__descriptor, protobuf_c_service_invoke_internal, NULL }
#define MT_GZPD800_T__GZPD800_TSERVICE__INIT(function_prefix__) \
    { MT_GZPD800_T__GZPD800_TSERVICE__BASE_INIT,\
      function_prefix__ ## get_data  }
void mt_gzpd800_t__gzpd800_tservice__get_data(ProtobufCService *service,
                                              const Google__Protobuf__Empty *input,
                                              MtGzpd800T__DataRes_Closure closure,
                                              void *closure_data);

/* --- descriptors --- */

extern const ProtobufCMessageDescriptor mt_gzpd800_t__data__descriptor;
extern const ProtobufCMessageDescriptor mt_gzpd800_t__data_res__descriptor;
extern const ProtobufCServiceDescriptor mt_gzpd800_t__gzpd800_tservice__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_mt_5fproto_5fgzpd800T_2eproto__INCLUDED */
