#pragma once

#import <AppKit/NSOpenGLView.h>
#import <Cocoa/Cocoa.h>
#import <CoreVideo/CoreVideo.h>
#import <OpenGL/gl.h>

#import "CCMessage.h"

#include "Plasmacore.h"

@interface CocoaCore : NSObject
{
  Plasmacore::Plasmacore plasmacore; 
  NSTimer*               update_timer;

  NSMutableDictionary*   message_callbacks;
  int                    next_callback_id;
}

+ (CocoaCore*) singleton;

//- (void) addListener:(NSString*)message_type 
- (void) addListener:(CCListener)listener forMessageType:(NSString*)type;
- (void) start;
- (void) stop;
- (void) update;

@end
