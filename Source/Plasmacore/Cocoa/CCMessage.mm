#import "CocoaCore.h"
#import "CCMessage.h"

#include "SuperCPPStringBuilder.h"
using namespace SuperCPP;

@interface CocoaCore()
- (CCMessage*) createMessage:(const char*)message_type;
- (int)        sendRSVP:(Plasmacore::Message)message withReplyListener:(CCListener)listener;
@end

@implementation CCMessage
+ (id) create:(const char*)message_type
{
  return [[CocoaCore singleton] createMessage:message_type];
}

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

- (CCMessage*) setString:  (const char*)name value:(NSString*)value
{
  StringBuilder buffer;
  buffer.reserve( (int) value.length );
  [value getCharacters:(unichar*)buffer.data range:NSMakeRange(0,value.length)];
  message.set_string( name, buffer );
  return self;
}

- (CCMessage*) setReal64:  (const char*)name value:(double)value
{
  message.set_real64( name, value );
  return self;
}

- (CCMessage*) setInt64:   (const char*)name value:(Int64)value
{
  message.set_int64( name, value );
  return self;
}

- (CCMessage*) setInt32:   (const char*)name value:(int)value
{
  message.set_int32( name, value );
  return self;
}

- (CCMessage*) setLogical: (const char*)name value:(bool)value
{
  message.set_logical( name, value );
  return self;
}

- (CCMessage*) setBytes:   (const char*)name value:(NSData*)value
{
  message.set_bytes( name, (Byte*) value.bytes, (int) value.length );
  return self;
}


- (bool) contains:(const char*)name
{
  return message.contains( name );
}

- (NSString*) getString:(const char*)name
{
  StringBuilder buffer;
  message.get_string( name, buffer );
  return [NSString stringWithCharacters:(unichar*)buffer.data length:buffer.count];
}

- (double) getReal64:(const char*)name
{
  return message.get_real64( name );
}

- (Int64) getInt64:(const char*)name
{
  return message.get_int64( name );
}

- (int) getInt32:(const char*)name
{
  return message.get_int32( name );
}

- (bool) getLogical:(const char*)name
{
  return message.get_logical( name );
}

- (NSData*)   getBytes:   (const char*)name
{
  Builder<Byte> buffer;
  message.get_bytes( name, buffer );
  return [NSData dataWithBytes:buffer.data length:buffer.count];
}

@end

