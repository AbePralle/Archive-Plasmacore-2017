#ifndef ROGUE_INTERFACE_H
#define ROGUE_INTERFACE_H

#include <stdint.h>

#define PLASMACORE_PLATFORM_SDL 1

#include "PlasmacoreList.h"

void    RogueInterface_configure();
void    RogueInterface_launch();
void    RogueInterface_send_messages( PlasmacoreList<uint8_t>& io );
void    RogueInterface_set_arg_count( int count );
void    RogueInterface_set_arg_value( int index, const char* value );

// Called by RogueInterface_configure() to set up sound.
bool    PlasmacoreSoundInit( void );

#endif // ROGUE_INTERFACE_H
