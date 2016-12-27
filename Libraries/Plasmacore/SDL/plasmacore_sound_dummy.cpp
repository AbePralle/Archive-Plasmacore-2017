#include "RogueProgram.h"
#include "rogue_interface.h"

void* PlasmacoreSound_create( RogueString* filepath, bool is_music )
{
  return 0;
}

void PlasmacoreSound_delete( void* sound )
{
}

double PlasmacoreSound_duration( void* sound )
{
  return 0.0;
}

bool PlasmacoreSound_is_playing( void* sound )
{
  return false;
}

void PlasmacoreSound_pause( void* sound )
{
}

double PlasmacoreSound_position( void* sound )
{
  return 0.0;
}

void PlasmacoreSound_play( void* sound, bool repeating )
{
}

void PlasmacoreSound_set_position( void* sound, double to_time )
{
  return;
}

void PlasmacoreSound_set_volume( void* sound, double volume )
{
}

bool PlasmacoreSoundInit( void )
{
  return true;
}
