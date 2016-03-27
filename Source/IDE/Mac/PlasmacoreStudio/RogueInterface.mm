#include "RogueInterface.h"
#include "RogueProgram.h"

#include <cstdio>
#include <cstring>
using namespace std;

static int          RogueInterface_argc = 0;
static const char** RogueInterface_argv = {0};

NSWindowController* Plasmacore_create_window( NSString* name )
{
  Class controller_class = NSClassFromString( name );
  if (controller_class == nil) controller_class = NSWindowController.self;

  NSWindowController* window = [[controller_class alloc] initWithWindowNibName:name];
  return window;
}

NSString* Plasmacore_rogue_string_to_ns_string( RogueString* st )
{
  if ( !st ) return nil;
  return [NSString stringWithCharacters:(unichar*)st->characters length:st->count];
}

RogueString* Plasmacore_ns_string_to_rogue_string( NSString* st )
{
  if ( !st ) return 0;

  RogueString* result = RogueString_create_with_count( (int)st.length );
  [st getCharacters:(unichar*)result->characters range:NSMakeRange(0,st.length)];
  return RogueString_update_hash_code( result );
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

