#import "CCResourceBank.h"

using namespace SuperCPP;

@implementation CCResourceBank

- (id) init
{
  resources = [[NSMutableArray alloc] init];
  [self clear];
  return self;
}

- (void) dealloc
{
  resources = nil;
}

- (int) addResource:(NSObject*)resource
{
  int resource_id = [self createID];
  [self setResource:resource withID:resource_id];
  return resource_id;
}

- (void) clear
{
  [resources removeAllObjects];
  active_ids.clear();
  available_ids.clear();
  [resources addObject:[NSNull null]];  // skip spot 0
}

- (int) createID
{
  int result;
  if (available_ids.count)
  {
    result = available_ids.remove_last();
  }
  else
  {
    result = (int) resources.count;
    [resources addObject:[NSNull null]];
  }
  active_ids.add( result );
  return result;
}

- (int) getCount
{
  return active_ids.count;
}

- (int) locateFirstResource
{
  if (active_ids.count == 0) return 0;
  return active_ids.data[0];
}

- (int) locateResource:(id)resource
{
  int       i = active_ids.count;
  while (--i >= 0)
  {
    if ([resources objectAtIndex:i] == resource) return i;
  }

  return -1;
}

- (id) getResourceWithID:(int)resource_id
{
  if (resource_id <= 0 || resource_id >= resources.count) return 0;
  return [resources objectAtIndex:resource_id];
}

- (id) removeResourceWithID:(int)resource_id
{
  int index;
  id  result;

  if (resource_id <= 0 || resource_id >= resources.count) return 0;

  available_ids.add( resource_id );

  result = [resources objectAtIndex:resource_id];
  [resources replaceObjectAtIndex:resource_id withObject:[NSNull null]];

  index = active_ids.locate( resource_id );
  if (index >= 0)
  {
    active_ids.data[index] = active_ids.data[ --active_ids.count ];
  }

  return result;
}

- (id) removeAnotherResource
{
  if (active_ids.count == 0) return 0;

  int resource_id = active_ids.data[0];
  if ( !resource_id ) return 0;

  return [self removeResourceWithID:resource_id];
}

- (id) setResource:(id) resource withID:(int)resource_id
{
  if (resource_id <= 0 || resource_id >= resources.count) return self;
  [resources replaceObjectAtIndex:resource_id withObject:resource];
  return self;
}

@end
