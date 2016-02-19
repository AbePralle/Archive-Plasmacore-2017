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
  [[CocoaCore singleton] addListener:^(int lid, id mesg){ NSLog(@"Got message of type %@ and value:%d\n",[mesg getType],[mesg getInt32:@"value"]); } forMessageType:@"Yin"];
  [[CocoaCore singleton] start];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification 
{
  [[CocoaCore singleton] stop];
}


@end
