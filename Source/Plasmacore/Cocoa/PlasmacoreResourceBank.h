#import <Cocoa/Cocoa.h>

#include "SuperCPPList.h"

@interface PlasmacoreResourceBank : NSObject 
{
  NSMutableDictionary* resources;
}

@property (nonatomic,readonly,getter=getCount) int count;

- (id)   init;
- (void) dealloc;

- (void) addResource:(id) resource withID:(int)resource_id;
- (void) clear;
- (int)  getCount;
- (int)  getIDOfResource:(id)resource;
- (id)   getResourceWithID:(int)resource_id;
- (id)   removeResourceWithID:(int)resource_id;
- (id)   removeAnotherResource;

@end
