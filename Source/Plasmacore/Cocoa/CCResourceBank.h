#import <Cocoa/Cocoa.h>

#include "SuperCPPList.h"

@interface CCResourceBank : NSObject 
{
  NSMutableArray*     resources;
  SuperCPP::List<int> active_ids;
  SuperCPP::List<int> available_ids;
}

@property (nonatomic,readonly,getter=getCount) int count;

- (id)   init;
- (void) dealloc;
- (int)  addResource:(NSObject*)resource;
- (void) clear;
- (int)  createID;
- (int)  getCount;
- (int)  locateFirstResource;
- (int)  locateResource:(id)resource;
- (id)   getResourceWithID:(int)resource_id;
- (id)   removeResourceWithID:(int)resource_id;
- (id)   removeAnotherResource;
- (id)   setResource:(id) resource withID:(int)resource_id;

@end
