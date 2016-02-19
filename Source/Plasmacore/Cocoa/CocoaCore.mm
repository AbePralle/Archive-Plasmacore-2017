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

- (int) addListener:(CCListener)listener forMessageType:(NSString*)type
{
  // Associate a unique integer listener_id with each listener that we can use
  // to track the listener in C++ code.  Returns the listener_id.
  int listener_id = next_callback_id;

  // Map the id to the listener
  [message_callbacks setObject:listener forKey:[NSNumber numberWithInt:listener_id]];

  // Map the id to the message type
  [listener_message_types setObject:type forKey:[NSNumber numberWithInt:listener_id]];

  // Pair the id with a C++ listener in C++ code
  plasmacore.message_manager.add_listener( [type UTF8String], CocoaCore_listener_callback, (void*)(intptr_t)listener_id );

  return listener_id;
}

- (CCListener) getListenerByID:(int)listener_id
{
  return [message_callbacks objectForKey:[NSNumber numberWithInt:listener_id]];
}

- (void) removeListenerByID:(int)listener_id
{
  NSNumber* key = [NSNumber numberWithInt:listener_id];

  [message_callbacks removeObjectForKey:key];

  NSString* type = [listener_message_types objectForKey:key];
  [listener_message_types removeObjectForKey:key];

  plasmacore.message_manager.remove_listener( [type UTF8String], CocoaCore_listener_callback, (void*)(intptr_t)listener_id );
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
  int listener_id = (int)(intptr_t)context;
  CCListener listener = [[CocoaCore singleton] getListenerByID:listener_id];
  if (listener) listener( listener_id, [[CCMessage alloc] initWithPlasmacoreMessage:m] );
}

