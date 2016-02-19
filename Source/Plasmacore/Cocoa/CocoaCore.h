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
  NSMutableDictionary*   listener_message_types;
  int                    next_callback_id;
}

+ (CocoaCore*) singleton;

- (int)  handleMessageType:(NSString*)type withListener:(CCListener)listener;
- (void) removeListenerByID:(int)listener_id;
- (void) start;
- (void) stop;
- (void) update;

@end
