//
//  AppDelegate.m
//  PlasmacoreStudio
//
//  Created by Abraham Pralle on 2/13/16.
//  Copyright © 2016 Abe Pralle. All rights reserved.
//

#import "AppDelegate.h"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
  [[CocoaCore singleton] handleMessageType:@"Yin"
    withListener:^(int this_id, id mesg)
    { 
      int value = [mesg getInt32:"value"];
      int m_id = [mesg getID];

      NSLog( @"Got message of type %@ and value:%d\n",[mesg getType],value );

      [[[CCMessage createReply:m_id] setInt32:"value" value:value+1] push];
    }
  ];

  [[CocoaCore singleton] start];

  [[CCMessage create:"Marco"]
    sendRSVP:^(int this_id, CCMessage* m)
    {
      NSLog( @"Got reply: %@\n", [m getString:"message"] );
    }
  ];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification 
{
  [[CocoaCore singleton] stop];
}


@end
