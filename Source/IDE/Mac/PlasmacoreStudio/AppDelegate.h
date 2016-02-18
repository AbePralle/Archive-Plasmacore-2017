//
//  AppDelegate.h
//  PlasmacoreStudio
//
//  Created by Abraham Pralle on 2/13/16.
//  Copyright © 2016 Abe Pralle. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CocoaPlasmacore.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>
{
  NSTimer*                update_timer;
  CocoaPlasmacore*        plasmacore;
  NSWindowController*     main_window;
}

- (void) startUpdateTimer;
- (void) stopUpdateTimer;
- (void) update;

@end

