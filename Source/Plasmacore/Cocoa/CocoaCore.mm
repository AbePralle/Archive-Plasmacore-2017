#import  "CocoaCore.h"

#include "RogueProgram.h"

/*
void yin_listener( Message m, void* context )
{
  printf( "%s (%d)\n", m.type, (int)(intptr_t)context );

  m.reply().set_int32( "value", m.get_int32("value") + 1 ).push();

}

void marco_callback( Message m, void* context )
{
  StringBuilder buffer;
  m.get_string( "message", buffer );
  printf( "%s\n", buffer.as_c_string() );

}
*/
static void CocoaCore_listener_callback( Plasmacore::Message m, void* context, void* data );

@implementation CocoaCore

+ (CocoaCore*) singleton
{
  static CocoaCore* the_singleton = nil;
  if ( !the_singleton )
  {
    the_singleton = [[CocoaCore alloc] init];
  }
  return the_singleton;
}

- (void) addListener:(CCListener)listener forMessageType:(NSString*)type
{
  int listener_id = next_callback_id;
  [message_callbacks setObject:listener forKey:[NSNumber numberWithInt:listener_id]];
  plasmacore.message_manager.add_listener( [type UTF8String], CocoaCore_listener_callback, (void*)(intptr_t)listener_id );
}

- (CCListener) getListenerForKey:(int)key
{
  return [message_callbacks objectForKey:[NSNumber numberWithInt:key]];
}

- (id) init
{
  self = [super self];
  if ( !self ) return nil;

  message_callbacks = [[NSMutableDictionary alloc] init];
  next_callback_id = 1;
 
  // Prepare command line arguments to pass in.
  NSArray *args = [[NSProcessInfo processInfo] arguments];
  int argc = (int) args.count;
  const char** argv = new const char*[ argc ];
  for (int i=0; i<argc; ++i)
  {
    argv[i] = [args[i] UTF8String];
  }
  
  // Configure and launch Rogue-generated C++ source code
  Rogue_configure( argc, argv );
  Rogue_launch();

  //plasmacore.message_manager.add_listener( "Yin", yin_listener, (void*) 3 );
  //plasmacore.message_manager.message( "Marco" ).send_rsvp( marco_callback );

  delete argv;

  return self;
}

- (void) start
{
  static NSWindowController* main_window = [[NSWindowController alloc] initWithWindowNibName:@"MainWindow"];
  [main_window showWindow:self];

  if ( !update_timer )
  {
    update_timer = [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(update)
                    userInfo:nil repeats:YES];
    [[NSRunLoop mainRunLoop] addTimer:update_timer forMode:NSRunLoopCommonModes];
  }
  
  [self update];
}

- (void) stop
{
  if (update_timer)
  {
    [update_timer invalidate];
    update_timer = nil;
  }
}

- (void) update
{
  plasmacore.message_manager.dispach_messages();
  if (plasmacore.message_manager.dispatch_requested)
  {
    // Another message wants to be dispatched already.  Make a one-off short interval
    // timer to handle that.
    [self performSelector:@selector(update) withObject:nil afterDelay:1.0/60];
  }
}

@end

static void CocoaCore_listener_callback( Plasmacore::Message m, void* context, void* data )
{
  int key = (int)(intptr_t)context;
  CCListener listener = [[CocoaCore singleton] getListenerForKey:key];
  if (listener) listener( [[CCMessage alloc] initWithPlasmacoreMessage:m] );
}

