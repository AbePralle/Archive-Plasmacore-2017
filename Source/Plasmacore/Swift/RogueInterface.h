#ifndef ROGUE_INTERFACE_H
#define ROGUE_INTERFACE_H

#import <Cocoa/Cocoa.h>

#ifdef __cplusplus
extern "C" {
#endif

void    RogueInterface_configure();
void    RogueInterface_launch();
NSData* RogueInterface_send_messages( const unsigned char* data, int count );
void    RogueInterface_set_arg_count( int count );
void    RogueInterface_set_arg_value( int index, const char* value );

int     Starbright_create_renderer();
void    Starbright_begin_draw( int renderer_id, int display_width, int display_height );
void    Starbright_end_draw( int renderer_id );

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ROGUE_INTERFACE_H
