#import "CocoaCore.h"
#import "CCMessage.h"

#include "SuperCPPStringBuilder.h"
using namespace SuperCPP;

@interface CocoaCore()
- (int) sendRSVP:(Plasmacore::Message)message withReplyListener:(CCListener)listener;
@end

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

- (CCMessage*) createReply
{
  return [[CCMessage alloc] initWithPlasmacoreMessage:message.create_reply()];
}

- (void) push
{
  message.push();
}

- (int) pushRSVP:(CCListener)callback
{
  int listener_id = [[CocoaCore singleton] sendRSVP:message withReplyListener:callback];
  [[CocoaCore singleton] update];
  return listener_id;
}

- (void) send
{
  message.send();
}

- (int) sendRSVP:(CCListener)callback
{
  return [[CocoaCore singleton] sendRSVP:message withReplyListener:callback];
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

- (NSData*)   getBytes:   (NSString*)name
{
  Builder<Byte> buffer;
  message.get_bytes( [name UTF8String], buffer );
  return [NSData dataWithBytes:buffer.data length:buffer.count];
}

@end

