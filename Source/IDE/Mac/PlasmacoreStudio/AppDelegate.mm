//
//  AppDelegate.m
//  PlasmacoreStudio
//
//  Created by Abraham Pralle on 2/13/16.
//  Copyright © 2016 Abe Pralle. All rights reserved.
//

#import "AppDelegate.h"
#include "RogueProgram.h"
#include "SuperCPPStringBuilder.h"
using namespace SuperCPP;

using namespace Messaging;

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

void yin_listener( Message m, void* context )
{
  printf( "%s (%d)\n", m.type, (int)(intptr_t)context );

  m.reply().set_int32( "value", m.get_int32("value") + 1 ).send();

}

void marco_callback( Message m, void* context )
{
  StringBuilder buffer;
  m.get_string( "message", buffer );
  printf( "%s\n", buffer.as_c_string() );

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

  message_manager.add_listener( "Yin", yin_listener, (void*) 3 );
  message_manager.message( "Marco" ).send_rsvp( marco_callback );
  message_manager.dispach_messages();
  message_manager.dispach_messages();

  delete argv;
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
  // Insert code here to tear down your application
}

@end
