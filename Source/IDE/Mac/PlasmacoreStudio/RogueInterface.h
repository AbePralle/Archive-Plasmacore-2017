#ifndef ROGUE_INTERFACE_H
#define ROGUE_INTERFACE_H

#import <Cocoa/Cocoa.h>

#ifdef __cplusplus
extern "C" {
#endif

NSWindowController* Plasmacore_create_window( NSString* name );

void    RogueInterface_configure();
void    RogueInterface_launch();
NSData* RogueInterface_send_messages( const unsigned char* data, int count );
void    RogueInterface_set_arg_count( int count );
void    RogueInterface_set_arg_value( int index, const char* value );

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ROGUE_INTERFACE_H
