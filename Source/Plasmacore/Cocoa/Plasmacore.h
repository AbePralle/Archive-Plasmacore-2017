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
  PLASMACORE::MessageManager message_manager; 
  NSTimer*               update_timer;

  NSMutableDictionary*   message_callbacks;
  NSMutableDictionary*   listener_message_types;
  int                    next_callback_id;

  PlasmacoreResourceBank*        resources;
}

+ (Plasmacore*) singleton;

- (int)  handleMessageType:(const char*)type withListener:(CCListener)listener;
- (void) removeListenerByID:(int)listener_id;
- (void) start;
- (void) stop;
- (void) update;

@end
