//=============================================================================
//  SuperCPP.h
//
//  v1.1.3 - 2016.03.05 by Abe Pralle
//
//  See README.md for instructions.
//=============================================================================
#ifndef SUPER_CPP_H
#define SUPER_CPP_H

#include <string.h>
#include <cinttypes>

#ifndef PROJECT_NAMESPACE
  #define PROJECT_NAMESPACE Project
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
  #define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _WIN64
  #define CPP_PLATFORM_WINDOWS         1
  #define CPP_PLATFORM_WINDOWS_64      1
#elif _WIN32
  #define CPP_PLATFORM_WINDOWS         1
  #define CPP_PLATFORM_WINDOWS_32      1
#elif __APPLE__
  #include "TargetConditionals.h"
  #if TARGET_IPHONE_SIMULATOR
    #define CPP_PLATFORM_IOS           1
    #define CPP_PLATFORM_IOS_SIMULATOR 1
  #elif TARGET_OS_IPHONE
    #define CPP_PLATFORM_IOS           1
    #define CPP_PLATFORM_IOS_DEVICE    1
  #else
    //   TARGET_OS_MAC
    #define CPP_PLATFORM_MAC           1
  #endif
#elif defined(ANDROID)
  #define CPP_PLATFORM_ANDROID         1
#elif defined(__linux__)
  #define CPP_PLATFORM_LINUX           1
  #define CPP_PLATFORM_UNIX            1
#elif defined(__unix__)
  #define CPP_PLATFORM_UNIX            1
#elif defined(_POSIX_VERSION)
  #define CPP_PLATFORM_POSIX           1
  #define CPP_PLATFORM_UNIX            1
#endif

namespace PROJECT_NAMESPACE
{
namespace SuperCPP
{

//=============================================================================
//  Type Defs
//=============================================================================
#if defined(_WIN32)
  typedef double  Real64;
  typedef float   Real32;
  typedef __int64 Int64;
  typedef __int32 Int32;
  typedef __int16 Int16;
  typedef __int8  Int8;
  typedef unsigned __int64 UInt64;
  typedef unsigned __int32 UInt32;
  typedef unsigned __int16 UInt16;
  typedef unsigned __int8  UInt8;
#else
  typedef double    Real64;
  typedef float     Real32;
  typedef int64_t   Int64;
  typedef int32_t   Int32;
  typedef int16_t   Int16;
  typedef int8_t    Int8;
  typedef uint64_t  UInt64;
  typedef uint32_t  UInt32;
  typedef uint16_t  UInt16;
  typedef uint8_t   UInt8;
#endif

typedef Real64 Real;
typedef Real32 Float;
typedef Int64  Long;
typedef Int32  Integer;
typedef UInt16 Character;
typedef UInt8  Byte;
typedef bool   Logical;

//=============================================================================
//  Utility 
//=============================================================================
double current_time_seconds();

int    decoded_utf8_count( const char* utf8_data, int utf8_count=-1 );
int    decoded_utf8_count( Character* utf8_data, int utf8_count );
void   decode_utf8( const char* utf8_data, int utf8_count, Character* dest_buffer );
void   decode_utf8( Character* utf8_data, int utf8_count, Character* dest_buffer );

void   double_to_c_string( double value, char* buffer, int size );

int    encoded_utf8_count( Character* characters, int count );
void   encode_utf8( Character* characters, int count, char* dest_buffer );
void   encode_utf8( Character* characters, int count, Character* dest_buffer );

} // namespace SuperCPP
} // namespace PROJECT_NAMESPACE

using namespace PROJECT_NAMESPACE;

#endif // SUPER_CPP_H
