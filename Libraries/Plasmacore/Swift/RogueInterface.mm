#include "RogueInterface.h"
#include "RogueProgram.h"

#import <AVFoundation/AVAudioPlayer.h>

#if defined(ROGUE_PLATFORM_IOS)
  #import <UIKit/UIKit.h>
#endif

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

RogueClassPlasmacore__Bitmap* Plasmacore_decode_image( RogueByte* bytes, RogueInt32 count )
{
  NSData* data = [NSData dataWithBytesNoCopy:bytes length:count freeWhenDone:NO];
#if defined(ROGUE_PLATFORM_IOS)
  CGImageRef bitmap_image = [UIImage imageWithData:data].CGImage;
#else
  CGImageRef bitmap_image = [[[NSImage alloc] initWithData:data] CGImageForProposedRect:NULL context:NULL hints:NULL];
#endif
  
  if(bitmap_image)
  {
    // Get the width and height of the image
    RogueInt32 width = (RogueInt32)CGImageGetWidth(bitmap_image);
    RogueInt32 height = (RogueInt32)CGImageGetHeight(bitmap_image);
    RogueClassPlasmacore__Bitmap* bitmap = RoguePlasmacore__Bitmap__create__Int32_Int32( width, height );

    // Uses the bitmap creation function provided by the Core Graphics framework.
    CGContextRef gc = CGBitmapContextCreate((GLubyte*)bitmap->pixels->data->as_int32s, width, height, 8, width * 4,
                                            CGColorSpaceCreateDeviceRGB(), kCGImageAlphaPremultipliedLast );
    CGContextDrawImage(gc, CGRectMake(0.0, 0.0, (CGFloat)width, (CGFloat)height), bitmap_image);
    CGContextRelease(gc);
    return bitmap;
  }
  else
  {
    return NULL;
  }

}

void* PlasmacoreSound_create( RogueString* filepath, bool is_music )
{
  NSString* ns_filepath = Plasmacore_rogue_string_to_ns_string( filepath );
  AVAudioPlayer* player = [[AVAudioPlayer alloc] initWithContentsOfURL:[NSURL fileURLWithPath:ns_filepath] error:nil];
  [player prepareToPlay];
  return (void*) CFBridgingRetain( player );
}

void PlasmacoreSound_delete( void* sound )
{
  if (sound) CFBridgingRelease( sound );
}

double PlasmacoreSound_duration( void* sound )
{
  if (sound)
  {
    AVAudioPlayer* player = (__bridge AVAudioPlayer*) sound;
    return (double) player.duration;
  }
  else
  {
    return 0.0;
  }
}

bool PlasmacoreSound_is_playing( void* sound )
{
  if (sound)
  {
    AVAudioPlayer* player = (__bridge AVAudioPlayer*) sound;
    return player.playing;
  }
  else
  {
    return false;
  }
}

void PlasmacoreSound_pause( void* sound )
{
  if (sound)
  {
    AVAudioPlayer* player = (__bridge AVAudioPlayer*) sound;
    [player pause];
  }
}

double PlasmacoreSound_position( void* sound )
{
  if (sound)
  {
    AVAudioPlayer* player = (__bridge AVAudioPlayer*) sound;
    return (double) player.currentTime;
  }
  else
  {
    return 0.0;
  }
}

void PlasmacoreSound_play( void* sound, bool repeating )
{
  if (sound)
  {
    AVAudioPlayer* player = (__bridge AVAudioPlayer*) sound;
    player.numberOfLoops = repeating ? -1 : 0;
    [player play];
  }
}

void PlasmacoreSound_set_position( void* sound, double to_time )
{
  if (sound)
  {
    AVAudioPlayer* player = (__bridge AVAudioPlayer*) sound;
    player.currentTime = (NSTimeInterval) to_time;
  }
}

void PlasmacoreSound_set_volume( void* sound, double volume )
{
  if (sound)
  {
    AVAudioPlayer* player = (__bridge AVAudioPlayer*) sound;
    player.volume = (float) volume;
  }
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

