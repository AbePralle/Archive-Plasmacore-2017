//
//  AppDelegate.m
//  PlasmacoreStudio
//
//  Created by Abraham Pralle on 2/13/16.
//  Copyright © 2016 Abe Pralle. All rights reserved.
//

#import "AppDelegate.h"
#import "PlasmacoreView.h"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
  [[Plasmacore singleton] start];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification 
{
  [[Plasmacore singleton] stop];
}


@end
