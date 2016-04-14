#include "RogueInterface.h"
#include "RogueProgram.h"
#include "Starbright.h"
#include "StarbrightGL.h"
#include "SuperCPPResourceBank.h"

#include <cstdio>
#include <cstring>
using namespace std;

static int          RogueInterface_argc = 0;
static const char** RogueInterface_argv = {0};

SuperCPP::ResourceBank<Starbright::Renderer*> starbright_renderers;

NSString* Plasmacore_rogue_string_to_ns_string( RogueString* st )
{
  if ( !st ) return nil;
  return [NSString stringWithUTF8String:(const char*)st->utf8];
}

RogueString* Plasmacore_ns_string_to_rogue_string( NSString* st )
{
  if ( !st ) return 0;

  RogueString* result = RogueString_create_from_utf8( [st UTF8String] );
  return RogueString_validate( result );
}

extern "C" RogueString* Plasmacore_find_asset( RogueString* filepath )
{
  NSString* ns_name = Plasmacore_rogue_string_to_ns_string( filepath );
  NSString* ns_filepath = [[NSBundle mainBundle] pathForResource:ns_name ofType:nil]; 
  if (ns_filepath == nil) return 0;
  return Plasmacore_ns_string_to_rogue_string( ns_filepath );
}

extern "C" void RogueInterface_configure()
{
  Rogue_configure( RogueInterface_argc, RogueInterface_argv );
}

extern "C" void RogueInterface_launch()
{
  Rogue_launch();
}

extern "C" NSData* RogueInterface_send_messages( const unsigned char* data, int count )
{
  RogueClassPlasmacore__MessageManager* mm =
    (RogueClassPlasmacore__MessageManager*) ROGUE_SINGLETON(Plasmacore__MessageManager);
  RogueByte_List* list = mm->io_buffer;

  RogueByte_List__clear( list );
  RogueByte_List__reserve__Int32( list, count );
  memcpy( list->data->bytes, data, count );
  list->count = count;

  // Call Rogue MessageManager.update(), which sends back a reference to another byte
  // list containing messages to us.
  list = RoguePlasmacore__MessageManager__update( mm );
  
  if ( !list ) return [NSData data];
  return [[NSData alloc] initWithBytes:list->data->bytes length:list->count];
}

extern "C" void RogueInterface_set_arg_count( int count )
{
  RogueInterface_argc = count;
  RogueInterface_argv = new const char*[ count+1 ];
  memset( RogueInterface_argv, 0, sizeof(const char*) * (count+1) );
}

extern "C" void RogueInterface_set_arg_value( int index, const char* value )
{
  size_t len = strlen( value );
  char* copy = new char[ len+1 ];
  strcpy( copy, value );
  RogueInterface_argv[ index ] = copy;
}

int Starbright_create_renderer()
{
  return starbright_renderers.add( new Starbright::GLRenderer() );
}

void Starbright_begin_draw( int renderer_id, int display_width, int display_height )
{
  Starbright::Renderer* renderer = starbright_renderers.get( renderer_id );
  if (renderer)
  {
    renderer->activate();
    renderer->begin_draw( display_width, display_height );
  }
}

void Starbright_end_draw( int renderer_id )
{
  Starbright::Renderer* renderer = starbright_renderers.get( renderer_id );
  if (renderer)
  {
    renderer->end_draw();
  }
}

