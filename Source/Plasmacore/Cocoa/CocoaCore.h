#pragma once

#import <AppKit/NSOpenGLView.h>
#import <Cocoa/Cocoa.h>
#import <CoreVideo/CoreVideo.h>
#import <OpenGL/gl.h>

#include "Plasmacore.h"

@interface CocoaCore : NSObject
{
  BOOL                   configured;
  Plasmacore::Plasmacore plasmacore; 
  NSTimer*               update_timer;
}

+ (CocoaCore*) singleton;
- (void)       start;
- (void)       stop;
- (void)       update;

@end
