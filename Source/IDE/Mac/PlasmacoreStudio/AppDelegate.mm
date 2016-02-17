//
//  AppDelegate.m
//  PlasmacoreStudio
//
//  Created by Abraham Pralle on 2/13/16.
//  Copyright © 2016 Abe Pralle. All rights reserved.
//

#import "AppDelegate.h"
#include "RogueProgram.h"

using namespace Messaging;

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

void test_callback( Message m, void* context )
{
  printf( "%s (%d)\n", m.type, (int)(intptr_t)context );

  m.reply().set_int32( "value", m.get_int32("value") + 1 ).send();

}

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
  // Prepare command line arguments to pass in.
  NSArray *args = [[NSProcessInfo processInfo] arguments];
  int argc = (int) args.count;
  const char** argv = new const char*[ argc ];
  for (int i=0; i<argc; ++i)
  {
    argv[i] = [args[i] UTF8String];
  }
  
  Rogue_configure( argc, argv );
  Rogue_launch();

  message_manager.add_listener( "Yin", test_callback, (void*) 3 );
  message_manager.dispach_messages();
  message_manager.dispach_messages();

  delete argv;
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
  // Insert code here to tear down your application
}

@end
