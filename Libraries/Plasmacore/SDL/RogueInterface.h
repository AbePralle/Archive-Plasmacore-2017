#ifndef ROGUE_INTERFACE_H
#define ROGUE_INTERFACE_H

#include <stdint.h>

#define PLASMACORE_PLATFORM_SDL 1

#include <vector>

void    RogueInterface_configure();
void    RogueInterface_launch();
void    RogueInterface_send_messages( const unsigned char* data, int count, std::vector<uint8_t> rx );
void    RogueInterface_set_arg_count( int count );
void    RogueInterface_set_arg_value( int index, const char* value );

// Called by RogueInterface_configure() to set up sound.
bool    PlasmacoreSoundInit( void );

#endif // ROGUE_INTERFACE_H
