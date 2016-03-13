#import <AppKit/NSOpenGLView.h>
#import <Cocoa/Cocoa.h>
#import <CoreVideo/CoreVideo.h>
#import <OpenGL/gl.h>

#import "PlasmacoreMessage.h"
#import "PlasmacoreResourceBank.h"
#import "PlasmacoreView.h"

#include "PlasmacoreMessageManager.h"
#include "RogueProgram.h"

@interface Plasmacore : NSObject
{
  PlasmacoreMessageManager* message_manager;
  NSTimer*                  update_timer;

  PlasmacoreResourceBank*   resources;
}

+ (Plasmacore*) singleton;

- (int)          addListenerForType:(const char*)message_type withCallback:(PlasmacoreCallback)callback;
- (void)         addResource:(id)resource withID:(int)resource_id;
- (int)          idOfResource:(id)resource;
- (RogueString*) nsStringToRogueString:(NSString*)st;
- (void)         removeListenerWithID:(int)listener_id;
- (id)           removeResource:(id)resource;
- (id)           removeResourceWithID:(int)resource_id;
- (id)           resourceWithID:(int)resource_id;
- (NSString*)    rogueStringToNSString:(RogueString*)st;
- (void)         start;
- (void)         stop;
- (void)         update;

@end

RogueString* Plasmacore_find_asset( RogueString* name, RogueString* extension );

