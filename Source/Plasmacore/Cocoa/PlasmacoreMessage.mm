#import "Plasmacore.h"
#import "PlasmacoreMessage.h"

#include "SuperCPPStringBuilder.h"
using namespace SuperCPP;

@interface Plasmacore()
- (PlasmacoreMessage*) createMessage:(const char*)message_type;
- (PlasmacoreMessage*) createReply:(int)message_id;
- (int)        sendRSVP:(PLASMACORE::Message)message withReplyListener:(CCListener)listener;
@end

@implementation PlasmacoreMessage
+ (id) messageWithType:(const char*)message_type
{
  return [[Plasmacore singleton] createMessage:message_type];
}

+ (id) messageWithReply:(int)message_id
{
  return [[Plasmacore singleton] createReply:message_id];
}

- (id) initWithManager:(PlasmacoreMessageManager*)_manager
{
  manager = _manager;
}

- (id) reset
{
  m_type = 0;
  m_id = 0;
  keys.clear();
  offsets.clear();
  buffer.reset();
  return self;
}

- (void) decodeIncomingMessageData:(char*)data size:(int)size
{
  [self reset];
  buffer.reserve( size );
  buffer.add( data, size );
  is_incoming = true;

  // TODO: index
}

- (NSString*) getType
{
  return [NSString stringWithUTF8String:message.m_type];
}

- (int) getID
{
  return message.id;
}

- (PlasmacoreMessage*) createReply
{
  return [[PlasmacoreMessage alloc] initWithPlasmacoreMessage:message.create_reply()];
}

- (void) push
{
  message.push();
}

- (int) pushRSVP:(CCListener)callback
{
  int listener_id = [[Plasmacore singleton] sendRSVP:message withReplyListener:callback];
  [[Plasmacore singleton] update];
  return listener_id;
}

- (void) send
{
  message.send();
}

- (int) sendRSVP:(CCListener)callback
{
  return [[Plasmacore singleton] sendRSVP:message withReplyListener:callback];
}

- (char*) data
{
  return (char*) buffer.data;
}

- (int) size
{
  return buffer.count;
}

- (PlasmacoreMessage*) setString:  (const char*)name value:(NSString*)value
{
  StringBuilder buffer;
  buffer.reserve( (int) value.length );
  [value getCharacters:(unichar*)buffer.data range:NSMakeRange(0,value.length)];
  buffer.count += (int) value.length;
  message.set_string( name, buffer );
  return self;
}

- (PlasmacoreMessage*) setReal64:  (const char*)name value:(double)value
{
  message.set_real64( name, value );
  return self;
}

- (PlasmacoreMessage*) setInt64:   (const char*)name value:(Int64)value
{
  message.set_int64( name, value );
  return self;
}

- (PlasmacoreMessage*) setInt32:   (const char*)name value:(int)value
{
  message.set_int32( name, value );
  return self;
}

- (PlasmacoreMessage*) setLogical: (const char*)name value:(bool)value
{
  message.set_logical( name, value );
  return self;
}

- (PlasmacoreMessage*) setBytes:   (const char*)name value:(NSData*)value
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

