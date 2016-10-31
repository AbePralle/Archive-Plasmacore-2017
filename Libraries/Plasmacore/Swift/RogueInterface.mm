#include "RogueInterface.h"
#include "RogueProgram.h"

#import <AVFoundation/AVAudioPlayer.h>

#include <cstdio>
#include <cstring>
using namespace std;

static int          RogueInterface_argc = 0;
static const char** RogueInterface_argv = {0};

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

extern "C" RogueString* Plasmacore_get_documents_folder()
{
  return Plasmacore_ns_string_to_rogue_string(
     [[[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject] path]
  );
}

extern "C" RogueString* Plasmacore_get_library_folder()
{
  return Plasmacore_ns_string_to_rogue_string(
     [[[[NSFileManager defaultManager] URLsForDirectory:NSLibraryDirectory inDomains:NSUserDomainMask] lastObject] path]
  );
}

extern "C" RogueString* Plasmacore_find_asset( RogueString* filepath )
{
  NSString* ns_name = Plasmacore_rogue_string_to_ns_string( filepath );
  NSString* ns_filepath = [[NSBundle mainBundle] pathForResource:ns_name ofType:nil];
  if (ns_filepath == nil) return 0;
  return Plasmacore_ns_string_to_rogue_string( ns_filepath );
}

void* PlasmacoreSound_create( RogueString* filepath, bool is_music )
{
  NSString* ns_filepath = Plasmacore_rogue_string_to_ns_string( filepath );
  AVAudioPlayer* player = [[AVAudioPlayer alloc] initWithContentsOfURL:[NSURL fileURLWithPath:ns_filepath] error:nil];
  [player prepareToPlay];
  return (void*) CFBridgingRetain( player );
}

void PlasmacoreSound_play( void* sound )
{
  if (sound)
  {
    AVAudioPlayer* player = (__bridge AVAudioPlayer*) sound;
    [player play];
  }
}

void PlasmacoreSound_delete( void* sound )
{
  if (sound) CFBridgingRelease( sound );
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
  try
  {
    Rogue_collect_garbage();

    RogueClassPlasmacore__MessageManager* mm =
      (RogueClassPlasmacore__MessageManager*) ROGUE_SINGLETON(Plasmacore__MessageManager);
    RogueByte_List* list = mm->io_buffer;

    RogueByte_List__clear( list );
    RogueByte_List__reserve__Int32( list, count );
    memcpy( list->data->as_bytes, data, count );
    list->count = count;

    // Call Rogue MessageManager.update(), which sends back a reference to another byte
    // list containing messages to us.
    list = RoguePlasmacore__MessageManager__update( mm );

    if ( !list ) return [NSData data];
    return [[NSData alloc] initWithBytes:list->data->as_bytes length:list->count];
  }
  catch (RogueException* err)
  {
    RogueException__display( err );
    return [NSData data];
  }
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

