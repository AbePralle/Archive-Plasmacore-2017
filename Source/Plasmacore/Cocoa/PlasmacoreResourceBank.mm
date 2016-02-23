#import "PlasmacoreResourceBank.h"

using namespace SuperCPP;

@implementation PlasmacoreResourceBank

- (id) init
{
  resources = [[NSMutableDictionary alloc] init];
  return self;
}

- (void) dealloc
{
  resources = nil;
}

- (void) addResource:(id)resource withID:(int)resource_id
{
  [resources setObject:resource forKey:[NSNumber numberWithInt:resource_id]];
}

- (void) clear
{
  [resources removeAllObjects];
}

- (int) getCount
{
  return (int)resources.count;
}

- (int) idOfResource:(id)resource
{
  NSEnumerator *enumerator = [resources keyEnumerator];
  NSNumber* key;
  while ((key = [enumerator nextObject]))
  {
    if ([resources objectForKey:key] == resource) return [key intValue];
  }
  return 0;
}

- (id) removeResourceWithID:(int)resource_id
{
  id result = [self resourceWithID:resource_id];
  [resources removeObjectForKey:[NSNumber numberWithInt:resource_id]];
  return result;
}

- (id) removeAnotherResource
{
  NSEnumerator *enumerator = [resources keyEnumerator];
  NSNumber* key;
  while ((key = [enumerator nextObject]))
  {
    return [self removeResourceWithID:[key intValue]];
  }

  return nil;
}

- (id) resourceWithID:(int)resource_id
{
  return [resources objectForKey:[NSNumber numberWithInt:resource_id]];
}


@end
