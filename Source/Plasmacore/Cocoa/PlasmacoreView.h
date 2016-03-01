#import <AppKit/NSOpenGLView.h>
#import <Cocoa/Cocoa.h>
#import <CoreVideo/CoreVideo.h>
#import <OpenGL/gl.h>

#import "StarbrightGLView.h"
using namespace Starbright;

@interface PlasmacoreView : StarbrightGLView
{
}

@property (nonatomic) NSString* name;

- (void)onCreate;
- (void)onDraw;
- (void)onStart;
- (void)onStop;

@end
