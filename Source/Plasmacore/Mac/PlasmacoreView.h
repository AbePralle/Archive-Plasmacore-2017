#pragma once

#import <AppKit/NSOpenGLView.h>
#import <Cocoa/Cocoa.h>
#import <CoreVideo/CoreVideo.h>
#import <OpenGL/gl.h>

#import "StarbrightGLView.h"

@interface PlasmacoreView : StarbrightGLView
{
}

- (void)onCreate;
- (void)onDraw;
- (void)onStart;
- (void)onStop;
- (void)onUpdate;

@end
