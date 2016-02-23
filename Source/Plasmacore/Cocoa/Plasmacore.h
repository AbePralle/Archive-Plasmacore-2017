#import <AppKit/NSOpenGLView.h>
#import <Cocoa/Cocoa.h>
#import <CoreVideo/CoreVideo.h>
#import <OpenGL/gl.h>

#import "PlasmacoreMessage.h"
#import "PlasmacoreResourceBank.h"
#import "PlasmacoreView.h"

#include "PlasmacoreMessageManager.h"

@interface Plasmacore : NSObject
{
  PlasmacoreMessageManager* message_manager;
  NSTimer*                  update_timer;

  PlasmacoreResourceBank*   resources;
}

+ (Plasmacore*) singleton;

- (int)  addListenerForType:(const char*)message_type withCallback:(PlasmacoreCallback)callback;
- (void) addResource:(id)resource withID:(int)resource_id;
- (int)  getResourceID:(id)resource;
- (id)   getResourceWithID:(int)resource_id;
- (void) removeListenerWithID:(int)listener_id;
- (id)   removeResource:(id)resource;
- (id)   removeResourceWithID:(int)resource_id;
- (void) start;
- (void) stop;
- (void) update;

@end
