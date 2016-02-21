#import  "Plasmacore.h"

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
static void CocoaCore_listener_callback( PLASMACORE::Message m, void* context, void* data );
static void CocoaCore_reply_callback( PLASMACORE::Message m, void* context, void* data );

@implementation Plasmacore

+ (Plasmacore*) singleton
{
  static Plasmacore* the_singleton = nil;
  if ( !the_singleton )
  {
    the_singleton = [[Plasmacore alloc] init];
  }
  return the_singleton;
}

- (id) init
{
  self = [super self];
  if ( !self ) return nil;

  resources = [[PlasmacoreResourceBank alloc] init];

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

  delete argv;

  // Set up standard message handlers
  [self handleMessageType:"Window.create"
    withListener:^(int this_id, PlasmacoreMessage* m)
    {
      NSString* window_name = [m getString:"name"];
      int       window_id   = [m getInt32:"id"];

      NSWindowController* window = [[NSClassFromString(window_name) alloc] initWithWindowNibName:window_name];
      [resources addResource:window withID:window_id];
    }
  ];

  [self handleMessageType:"Window.show"
    withListener:^(int this_id, PlasmacoreMessage* m)
    {
      int window_id = [m getInt32:"id"];

      NSWindowController* window = [resources getResourceWithID:window_id];
      if (window) [window showWindow:self];
    }
  ];

  [self handleMessageType:"Window.call"
    withListener:^(int this_id, PlasmacoreMessage* m)
    {
      int window_id = [m getInt32:"id"];

      NSWindowController* window = [resources getResourceWithID:window_id];
      if (window)
      {
        SEL selector = NSSelectorFromString( [[m getString:"method_name"] stringByAppendingString:@":"] );
        if ([window respondsToSelector:selector])
        ((void (*)(id, SEL, PlasmacoreMessage*))[window methodForSelector:selector])(window, selector, m);
      }
    }
  ];

  [self handleMessageType:"Window.close"
    withListener:^(int this_id, PlasmacoreMessage* m)
    {
      int window_id = [m getInt32:"id"];

      NSWindowController* window = [resources removeResourceWithID:window_id];
      if (window) [window close];
    }
  ];

  return self;
}

- (PlasmacoreMessage*) createMessage:(const char*)message_type
{
  return [[PlasmacoreMessage alloc] initWithPlasmacoreMessage:message_manager.create_message(message_type)];
}

- (PlasmacoreMessage*) createReply:(int)message_id
{
  return [[PlasmacoreMessage alloc] initWithPlasmacoreMessage:message_manager.create_message("<reply>",message_id)];
}

- (int) handleMessageType:(const char*)type withListener:(CCListener)listener
{
  // Associate a unique integer listener_id with each listener that we can use
  // to track the listener in C++ code.  Returns the listener_id.
  int listener_id = next_callback_id++;

  // Map the id to the listener
  [message_callbacks setObject:listener forKey:[NSNumber numberWithInt:listener_id]];

  // Map the id to the message type
  [listener_message_types setObject:[NSString stringWithUTF8String:type] forKey:[NSNumber numberWithInt:listener_id]];

  // Pair the id with a C++ listener in C++ code
  message_manager.add_listener( type, CocoaCore_listener_callback, (void*)(intptr_t)listener_id );

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

  message_manager.remove_listener( [type UTF8String], CocoaCore_listener_callback, (void*)(intptr_t)listener_id );
}

- (CCListener) removeReplyListenerByID:(int)listener_id
{
  NSNumber* key = [NSNumber numberWithInt:listener_id];

  CCListener listener = [message_callbacks objectForKey:key];
  if (listener) [message_callbacks removeObjectForKey:key];
  return listener;
}

- (int) sendRSVP:(PLASMACORE::Message)message withReplyListener:(CCListener)listener
{
  // Associate a unique integer listener_id with each listener that we can use
  // to track the listener in C++ code.  Returns the listener_id.
  int listener_id = next_callback_id;

  // Map the id to the listener
  [message_callbacks setObject:listener forKey:[NSNumber numberWithInt:listener_id]];

  message.send_rsvp( CocoaCore_reply_callback, (void*)(intptr_t)listener_id );

  return listener_id;
}

- (void) start
{
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
  message_manager.dispach_messages();

  // Dispatch pushed messages up to 10 times in a row to facilitate lightweight back-and-forth
  // communication.
  for (int i=0; i<10; ++i)
  {
    if ( !message_manager.dispatch_requested ) return;
    message_manager.dispach_messages();
  }

  // Gotta give it a break sometime - schedule another round of updates in 1/60 sec if
  // there are still waiting messages.
  if (message_manager.dispatch_requested)
  {
    [self performSelector:@selector(update) withObject:nil afterDelay:1.0/60];
  }
}

@end

static void CocoaCore_listener_callback( PLASMACORE::Message m, void* context, void* data )
{
  int listener_id = (int)(intptr_t)context;
  CCListener listener = [[Plasmacore singleton] getListenerByID:listener_id];
  if (listener) listener( listener_id, [[PlasmacoreMessage alloc] initWithPlasmacoreMessage:m] );
}

static void CocoaCore_reply_callback( PLASMACORE::Message m, void* context, void* data )
{
  int listener_id = (int)(intptr_t)context;
  CCListener listener = [[Plasmacore singleton] removeReplyListenerByID:listener_id];
  if (listener) listener( listener_id, [[PlasmacoreMessage alloc] initWithPlasmacoreMessage:m] );
}

