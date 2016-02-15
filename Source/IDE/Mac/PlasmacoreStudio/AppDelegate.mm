//
//  AppDelegate.m
//  PlasmacoreStudio
//
//  Created by Abraham Pralle on 2/13/16.
//  Copyright © 2016 Abe Pralle. All rights reserved.
//

#import "AppDelegate.h"
#include "RogueProgram.h"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

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

  message_manager.message( "Hello World!" ).send();
  message_manager.dispach_messages();

  delete argv;
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
  // Insert code here to tear down your application
}

@end
