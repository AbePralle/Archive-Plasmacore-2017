#ifndef ROGUE_INTERFACE_H
#define ROGUE_INTERFACE_H

#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
  #define PLASMACORE_PLATFORM_IOS           1
  #define PLASMACORE_PLATFORM_IOS_SIMULATOR 1
#elif TARGET_OS_IPHONE
  #define PLASMACORE_PLATFORM_IOS           1
  #define PLASMACORE_PLATFORM_IOS_DEVICE    1
#else
  #define PLASMACORE_PLATFORM_MAC           1
#endif

#ifdef PLASMACORE_PLATFORM_MAC
  #import <Cocoa/Cocoa.h>
#else
  #import <Foundation/Foundation.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

void    RogueInterface_configure();
void    RogueInterface_launch();
NSData* RogueInterface_send_messages( const unsigned char* data, int count );
void    RogueInterface_set_arg_count( int count );
void    RogueInterface_set_arg_value( int index, const char* value );

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ROGUE_INTERFACE_H
