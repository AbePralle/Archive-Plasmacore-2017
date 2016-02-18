#pragma once

#import <AppKit/NSOpenGLView.h>
#import <Cocoa/Cocoa.h>
#import <CoreVideo/CoreVideo.h>
#import <OpenGL/gl.h>

#include "Plasmacore.h"

@interface CocoaPlasmacore : NSObject
{
  Plasmacore::Plasmacore plasmacore; 
}

@end
