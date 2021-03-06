/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: mt_proto_jxyl001.proto */

#ifndef PROTOBUF_C_mt_5fproto_5fjxyl001_2eproto__INCLUDED
#define PROTOBUF_C_mt_5fproto_5fjxyl001_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1003000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1003001 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif

#include "google/protobuf/empty.pb-c.h"

typedef struct _MtJxyl001__Temp MtJxyl001__Temp;
typedef struct _MtJxyl001__Fog MtJxyl001__Fog;
typedef struct _MtJxyl001__DataRes MtJxyl001__DataRes;


/* --- enums --- */


/* --- messages --- */

struct  _MtJxyl001__Temp
{
  ProtobufCMessage base;
  /*
   * 温湿度地址
   */
  int32_t addr;
  /*
   * 温度
   */
  double temp;
  /*
   * 湿度
   */
  double hum;
  /*
   * 温湿度传感器电量百分比
   */
  double battemp;
};
#define MT_JXYL001__TEMP__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_jxyl001__temp__descriptor) \
    , 0, 0, 0, 0 }


struct  _MtJxyl001__Fog
{
  ProtobufCMessage base;
  /*
   * 烟雾地址
   */
  int32_t addr;
  /*
   * 烟雾
   */
  double frog;
  /*
   * 烟雾传感器电量百分比
   */
  double batfog;
};
#define MT_JXYL001__FOG__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_jxyl001__fog__descriptor) \
    , 0, 0, 0 }


struct  _MtJxyl001__DataRes
{
  ProtobufCMessage base;
  int32_t rescode;
  size_t n_temps;
  MtJxyl001__Temp **temps;
  size_t n_fogs;
  MtJxyl001__Fog **fogs;
};
#define MT_JXYL001__DATA_RES__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mt_jxyl001__data_res__descriptor) \
    , 0, 0,NULL, 0,NULL }


/* MtJxyl001__Temp methods */
void   mt_jxyl001__temp__init
                     (MtJxyl001__Temp         *message);
size_t mt_jxyl001__temp__get_packed_size
                     (const MtJxyl001__Temp   *message);
size_t mt_jxyl001__temp__pack
                     (const MtJxyl001__Temp   *message,
                      uint8_t             *out);
size_t mt_jxyl001__temp__pack_to_buffer
                     (const MtJxyl001__Temp   *message,
                      ProtobufCBuffer     *buffer);
MtJxyl001__Temp *
       mt_jxyl001__temp__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_jxyl001__temp__free_unpacked
                     (MtJxyl001__Temp *message,
                      ProtobufCAllocator *allocator);
/* MtJxyl001__Fog methods */
void   mt_jxyl001__fog__init
                     (MtJxyl001__Fog         *message);
size_t mt_jxyl001__fog__get_packed_size
                     (const MtJxyl001__Fog   *message);
size_t mt_jxyl001__fog__pack
                     (const MtJxyl001__Fog   *message,
                      uint8_t             *out);
size_t mt_jxyl001__fog__pack_to_buffer
                     (const MtJxyl001__Fog   *message,
                      ProtobufCBuffer     *buffer);
MtJxyl001__Fog *
       mt_jxyl001__fog__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_jxyl001__fog__free_unpacked
                     (MtJxyl001__Fog *message,
                      ProtobufCAllocator *allocator);
/* MtJxyl001__DataRes methods */
void   mt_jxyl001__data_res__init
                     (MtJxyl001__DataRes         *message);
size_t mt_jxyl001__data_res__get_packed_size
                     (const MtJxyl001__DataRes   *message);
size_t mt_jxyl001__data_res__pack
                     (const MtJxyl001__DataRes   *message,
                      uint8_t             *out);
size_t mt_jxyl001__data_res__pack_to_buffer
                     (const MtJxyl001__DataRes   *message,
                      ProtobufCBuffer     *buffer);
MtJxyl001__DataRes *
       mt_jxyl001__data_res__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mt_jxyl001__data_res__free_unpacked
                     (MtJxyl001__DataRes *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*MtJxyl001__Temp_Closure)
                 (const MtJxyl001__Temp *message,
                  void *closure_data);
typedef void (*MtJxyl001__Fog_Closure)
                 (const MtJxyl001__Fog *message,
                  void *closure_data);
typedef void (*MtJxyl001__DataRes_Closure)
                 (const MtJxyl001__DataRes *message,
                  void *closure_data);

/* --- services --- */

typedef struct _MtJxyl001__Jxyl001Service_Service MtJxyl001__Jxyl001Service_Service;
struct _MtJxyl001__Jxyl001Service_Service
{
  ProtobufCService base;
  void (*get_data)(MtJxyl001__Jxyl001Service_Service *service,
                   const Google__Protobuf__Empty *input,
                   MtJxyl001__DataRes_Closure closure,
                   void *closure_data);
};
typedef void (*MtJxyl001__Jxyl001Service_ServiceDestroy)(MtJxyl001__Jxyl001Service_Service *);
void mt_jxyl001__jxyl001_service__init (MtJxyl001__Jxyl001Service_Service *service,
                                        MtJxyl001__Jxyl001Service_ServiceDestroy destroy);
#define MT_JXYL001__JXYL001_SERVICE__BASE_INIT \
    { &mt_jxyl001__jxyl001_service__descriptor, protobuf_c_service_invoke_internal, NULL }
#define MT_JXYL001__JXYL001_SERVICE__INIT(function_prefix__) \
    { MT_JXYL001__JXYL001_SERVICE__BASE_INIT,\
      function_prefix__ ## get_data  }
void mt_jxyl001__jxyl001_service__get_data(ProtobufCService *service,
                                           const Google__Protobuf__Empty *input,
                                           MtJxyl001__DataRes_Closure closure,
                                           void *closure_data);

/* --- descriptors --- */

extern const ProtobufCMessageDescriptor mt_jxyl001__temp__descriptor;
extern const ProtobufCMessageDescriptor mt_jxyl001__fog__descriptor;
extern const ProtobufCMessageDescriptor mt_jxyl001__data_res__descriptor;
extern const ProtobufCServiceDescriptor mt_jxyl001__jxyl001_service__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_mt_5fproto_5fjxyl001_2eproto__INCLUDED */
