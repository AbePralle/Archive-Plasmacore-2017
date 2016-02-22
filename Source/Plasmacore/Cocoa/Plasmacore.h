#import <AppKit/NSOpenGLView.h>
#import <Cocoa/Cocoa.h>
#import <CoreVideo/CoreVideo.h>
#import <OpenGL/gl.h>

#import "PlasmacoreMessage.h"
#import "PlasmacoreResourceBank.h"
#import "PlasmacoreView.h"

#include "PlasmacoreMessaging.h"

@interface Plasmacore : NSObject
{
  PlasmacoreMessageManager message_manager; 
  NSTimer*                 update_timer;

  PlasmacoreResourceBank*  resources;
}

+ (Plasmacore*) singleton;

- (int)  addListenerForType:(const char*)message_type withCallback:(PlasmacoreCallback)callback;
- (int)  getWindowID:(id)window;
- (void) removeListenerByID:(int)listener_id;
- (void) start;
- (void) stop;
- (void) update;

@end
