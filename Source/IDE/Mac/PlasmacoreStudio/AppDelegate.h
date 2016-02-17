//
//  AppDelegate.h
//  PlasmacoreStudio
//
//  Created by Abraham Pralle on 2/13/16.
//  Copyright © 2016 Abe Pralle. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include "Messaging.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>
{
  Messaging::Manager message_manager;
}

@end

