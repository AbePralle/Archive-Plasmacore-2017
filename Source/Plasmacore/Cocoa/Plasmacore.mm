#import  "Plasmacore.h"

#include "RogueProgram.h"

@implementation Plasmacore

RogueString* Plasmacore_find_asset( RogueString* name, RogueString* extension )
{
  Plasmacore* plasmacore = [Plasmacore singleton];
  NSString* ns_name = [plasmacore rogueStringToNSString:name];
  NSString* ns_extension = [plasmacore rogueStringToNSString:extension];
  NSString* ns_filepath = [[NSBundle mainBundle] pathForResource:ns_name ofType:ns_extension]; 
  return [plasmacore nsStringToRogueString:ns_filepath];
}

+ (Plasmacore*) singleton
{
  static Plasmacore* the_singleton = nil;
  if ( !the_singleton )
  {
    the_singleton = [Plasmacore alloc];
    the_singleton = [the_singleton init];
  }
  return the_singleton;
}

- (id) init
{
  self = [super self];
  if ( !self ) return nil;

  resources = [[PlasmacoreResourceBank alloc] init];
  message_manager = [[PlasmacoreMessageManager alloc] init];

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
  [self addListenerForType:"Window.create"
    withCallback:^(int this_id, PlasmacoreMessage* m)
    {
      NSString* window_name = [m getString:"name"];
      int       window_id   = [m getInt32:"id"];

      NSWindowController* window = [[NSClassFromString(window_name) alloc] initWithWindowNibName:window_name];
      [[Plasmacore singleton] addResource:window withID:window_id];
    }
  ];

  [self addListenerForType:"Window.show"
    withCallback:^(int this_id, PlasmacoreMessage* m)
    {
      int window_id = [m getInt32:"id"];

      NSWindowController* window = [[Plasmacore singleton] resourceWithID:window_id];
      if (window) [window showWindow:[Plasmacore singleton]];
    }
  ];

  [self addListenerForType:"Window.call"
    withCallback:^(int this_id, PlasmacoreMessage* m)
    {
      int window_id = [m getInt32:"id"];

      NSWindowController* window = [[Plasmacore singleton] resourceWithID:window_id];
      if (window)
      {
        SEL selector = NSSelectorFromString( [[m getString:"method_name"] stringByAppendingString:@":"] );
        if ([window respondsToSelector:selector])
        ((void (*)(id, SEL, PlasmacoreMessage*))[window methodForSelector:selector])(window, selector, m);
      }
    }
  ];

  [self addListenerForType:"Window.close"
    withCallback:^(int this_id, PlasmacoreMessage* m)
    {
      int window_id = [m getInt32:"id"];

      NSWindowController* window = [[Plasmacore singleton] removeResourceWithID:window_id];
      if (window) [window close];
    }
  ];

  [[PlasmacoreMessage messageWithType:"App.launch"] send];

  return self;
}

- (int)  addListenerForType:(const char*)message_type withCallback:(PlasmacoreCallback)callback;
{
  return [message_manager addListenerForType:message_type withCallback:callback];
}

- (void) addResource:(id)resource withID:(int)resource_id
{
  [resources addResource:resource withID:resource_id];
}

- (int) idOfResource:(id)resource
{
  return [resources idOfResource:resource];
}

- (RogueString*) nsStringToRogueString:(NSString*)st
{
  RogueString* result;
  @synchronized (message_manager)
  {
    result = RogueString_create_with_count( (int)st.length );
  }
  [st getCharacters:(unichar*)result->characters range:NSMakeRange(0,st.length)];
  return RogueString_update_hash_code( result );
}

- (PlasmacoreMessage*) obtainMessage
{
  return [message_manager obtainMessage];
}

- (void) removeListenerWithID:(int)listener_id
{
  [message_manager removeListenerWithID:listener_id];
}

- (id) removeResource:(id)resource
{
  return [self removeResourceWithID:[self idOfResource:resource]];
}

- (id) removeResourceWithID:(int)resource_id
{
  return [resources removeResourceWithID:resource_id];
}

- (id) resourceWithID:(int)resource_id
{
  return [resources resourceWithID:resource_id];
}

- (NSString*) rogueStringToNSString:(RogueString*)st
{
  if ( !st ) return nil;
  return [NSString stringWithCharacters:(unichar*)st->characters length:st->count];
}

//- (void) sendTestMessage
//{
//  [[PlasmacoreMessage messageWithType:"test"] send];
//}

- (void) start
{
  [[PlasmacoreMessage messageWithType:"App.start"] send];

  if ( !update_timer )
  {
    //static NSTimer* test_timer = [NSTimer scheduledTimerWithTimeInterval:0.01 target:self selector:@selector(sendTestMessage)
    //                userInfo:nil repeats:YES];
    //[[NSRunLoop mainRunLoop] addTimer:test_timer forMode:NSRunLoopCommonModes];

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

  [[PlasmacoreMessage messageWithType:"App.start"] push];
}

- (void) update
{
  [message_manager dispatchMessages];

  // Dispatch pushed messages up to 10 times in a row to facilitate lightweight back-and-forth
  // communication.
  for (int i=0; i<10; ++i)
  {
    if ( !message_manager.dispatchRequested ) return;
    [message_manager dispatchMessages];
  }

  // Gotta give it a break sometime - schedule another round of updates in 1/60 sec if
  // there are still waiting messages.
  if (message_manager.dispatchRequested)
  {
    [self performSelector:@selector(update) withObject:nil afterDelay:1.0/60];
  }
}

@end

