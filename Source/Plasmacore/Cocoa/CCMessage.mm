#import "CCMessage.h"

#include "SuperCPPStringBuilder.h"
using namespace SuperCPP;

@implementation CCMessage
- (id) initWithPlasmacoreMessage:(Plasmacore::Message)m
{
  self = [super init];
  if (self) message = m;
  return self;
}

- (NSString*) getType
{
  return [NSString stringWithUTF8String:message.type];
}

- (void) push
{
  message.push();
}

- (void) push_rsvp:(CCListener)callback
{
  message.push();
}

- (bool) contains:(NSString*)name
{
  return message.contains( [name UTF8String] );
}

- (NSString*) getString:(NSString*)name
{
  StringBuilder buffer;
  message.get_string( [name UTF8String], buffer );
  return [NSString stringWithUTF8String:buffer.as_utf8_string()];
}

- (double) getReal64:(NSString*)name
{
  return message.get_real64( [name UTF8String] );
}

- (Int64) getInt64:(NSString*)name
{
  return message.get_int64( [name UTF8String] );
}

- (int) getInt32:(NSString*)name
{
  return message.get_int32( [name UTF8String] );
}

- (bool) getLogical:(NSString*)name
{
  return message.get_logical( [name UTF8String] );
}

- (NSData*)   getByteList:   (NSString*)name
{
  Builder<Byte> buffer;
  message.get_byte_list( [name UTF8String], buffer );
  return [NSData dataWithBytes:buffer.data length:buffer.count];
}

- (NSData*)   getReal64List: (NSString*)name
{
  Builder<Real64> buffer;
  message.get_real64_list( [name UTF8String], buffer );
  return [NSData dataWithBytes:(char*)buffer.data length:buffer.count];
}

- (NSData*)   getInt32List:  (NSString*)name
{
  Builder<Int32> buffer;
  message.get_int32_list( [name UTF8String], buffer );
  return [NSData dataWithBytes:(char*)buffer.data length:buffer.count];
}

@end

