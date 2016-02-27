#import "Plasmacore.h"
#import "PlasmacoreMessage.h"

#include "SuperCPPStringBuilder.h"
using namespace SuperCPP;

static int Plasmacore_next_outgoing_message_id = 1;

typedef enum
{
  PLASMACORE_MESSAGE_DATA_TYPE_STRING  = 1,
  PLASMACORE_MESSAGE_DATA_TYPE_REAL64  = 2,
  PLASMACORE_MESSAGE_DATA_TYPE_INT64   = 3,
  PLASMACORE_MESSAGE_DATA_TYPE_INT32   = 4,
  PLASMACORE_MESSAGE_DATA_TYPE_LOGICAL = 5,
  PLASMACORE_MESSAGE_DATA_TYPE_BYTES   = 6
} PlasmacoreMessageDataType;

@interface Plasmacore()
- (PlasmacoreMessage*) obtainMessage;
@end

@implementation PlasmacoreMessage
- (int) writeID:(const char*)name
{
  int _id = [manager outgoingNameToID:name];
  buffer.write_int32x( _id );
  return _id;
}

+ (id) messageWithType:(const char*)message_type
{
  return [PlasmacoreMessage messageWithType:message_type withMessageID:Plasmacore_next_outgoing_message_id++];
}

+ (id) messageWithType:(const char*)message_type withMessageID:(int)m_id
{
  PlasmacoreMessage* m = [[Plasmacore singleton] obtainMessage];
  [m reset];
  m->is_incoming = false;
  m->_messageID = m_id;
  m->_typeID = [m writeID:message_type];
  m->buffer.write_int32x( m->_messageID );
  return m;
}

- (id) initWithManager:(PlasmacoreMessageManager*)_manager
{
  if ( !(self = [super init]) ) return nil;
  manager = _manager;
  return self;
}

- (id) reset
{
  _typeID = 0;
  _messageID = 0;
  keys.clear();
  offsets.clear();
  buffer.reset();
  return self;
}

- (void) indexAnother:(DataReader*)reader
{
  keys.add( reader->read_int32x() );
  offsets.add( reader->position );
  
  int data_type = reader->read_int32x();
  switch (data_type)
  {
    case PLASMACORE_MESSAGE_DATA_TYPE_STRING:
    case PLASMACORE_MESSAGE_DATA_TYPE_BYTES:
    {
      StringBuilder temp_buffer;
      reader->read_string( temp_buffer );
      return;
    }
      
    case PLASMACORE_MESSAGE_DATA_TYPE_REAL64:
      reader->read_real64();
      return;
      
    case PLASMACORE_MESSAGE_DATA_TYPE_INT64:
      reader->read_int64x();
      return;
      
    case PLASMACORE_MESSAGE_DATA_TYPE_INT32:
    case PLASMACORE_MESSAGE_DATA_TYPE_LOGICAL:
      reader->read_int32x();
      return;
      
    default:
      NSLog( @"ERROR: unknown data type '%d' (%c) reading incoming message in native layer.\n", data_type, (char)data_type );
      return;
  }
}

- (void) decodeIncomingMessageData:(char*)data size:(int)size
{
  [self reset];
  buffer.reserve( size );
  buffer.add( (Byte*)data, size );
  is_incoming = true;

  DataReader reader( buffer.data, buffer.count );
  _typeID = reader.read_int32x();
  _messageID   = reader.read_int32x();
  while (reader.has_another())
  {
    [self indexAnother:&reader];
  }
}

- (NSString*) messageType
{
  const char* name;
  if (is_incoming) name = [manager incomingIDToName:_typeID];
  else             name = [manager outgoingIDToName:_typeID];

  return [NSString stringWithUTF8String:name];
}

- (PlasmacoreMessage*) createReply
{
  return [PlasmacoreMessage messageWithType:"<reply>" withMessageID:_messageID];
}

- (void) push
{
  [manager send:self];
  [manager dispatchMessages];
}

- (void) pushRSVP:(PlasmacoreCallback)callback
{
  [manager sendRSVP:self withCallback:callback];
  [manager dispatchMessages];
}

- (void) send
{
  [manager send:self];
}

- (void) sendRSVP:(PlasmacoreCallback)callback
{
  [manager sendRSVP:self withCallback:callback];
}

- (char*) data
{
  return (char*) buffer.data;
}

- (int) size
{
  return buffer.count;
}


- (PlasmacoreMessage*) setCString:  (const char*)name value:(const char*)value
{
  [self writeID:name];
  buffer.write_int32x( PLASMACORE_MESSAGE_DATA_TYPE_STRING );
  buffer.write_string( value );
  return self;
}

- (PlasmacoreMessage*) setString:  (const char*)name value:(NSString*)value
{
  [self writeID:name];
  buffer.write_int32x( PLASMACORE_MESSAGE_DATA_TYPE_STRING );

  StringBuilder string_buffer;
  string_buffer.reserve( (int) value.length );
  [value getCharacters:(unichar*)string_buffer.data range:NSMakeRange(0,value.length)];
  string_buffer.count += (int) value.length;

  buffer.write_string( string_buffer.data, string_buffer.count );
  return self;
}

- (PlasmacoreMessage*) setReal64:  (const char*)name value:(double)value
{
  [self writeID:name];
  buffer.write_int32x( PLASMACORE_MESSAGE_DATA_TYPE_REAL64 );
  buffer.write_real64( value );
  return self;
}

- (PlasmacoreMessage*) setInt64:   (const char*)name value:(Int64)value
{
  [self writeID:name];
  buffer.write_int32x( PLASMACORE_MESSAGE_DATA_TYPE_INT64 );
  buffer.write_int64x( value );
  return self;
}

- (PlasmacoreMessage*) setInt32:   (const char*)name value:(int)value
{
  [self writeID:name];
  buffer.write_int32x( PLASMACORE_MESSAGE_DATA_TYPE_INT32 );
  buffer.write_int32x( value );
  return self;
}

- (PlasmacoreMessage*) setLogical: (const char*)name value:(bool)value
{
  [self writeID:name];
  buffer.write_int32x( PLASMACORE_MESSAGE_DATA_TYPE_LOGICAL );
  buffer.write_int32x( value ? 1 : 0 );
  return self;
}

- (PlasmacoreMessage*) setBytes:   (const char*)name value:(NSData*)value
{
  [self writeID:name];
  buffer.write_int32x( PLASMACORE_MESSAGE_DATA_TYPE_BYTES );

  int count = (int) value.length;
  buffer.write_int32x( count );
  Byte* bytes = (Byte*) value.bytes;
  for (int i=0; i<count; ++i)
  {
    buffer.write_byte( bytes[i] );
  }
  return self;
}


- (int) locateKey:(const char*)name
{
  int key = [manager incomingNameToID:name];
  if ( !key ) return -1;

  int i = keys.count;
  while (--i >= 0)
  {
    if (keys[i] == key) return offsets[ i ];
  }
  return -1;
}

- (bool) contains:(const char*)name
{
  return [self locateKey:name] != -1;
}

- (NSString*) getString:(const char*)name
{
  int offset = [self locateKey:name];
  if (offset == -1) return @"";

  DataReader reader( buffer.data, buffer.count );
  reader.position = offset;

  if (reader.read_int32x() != PLASMACORE_MESSAGE_DATA_TYPE_STRING) return @"";

  StringBuilder string_buffer;
  reader.read_string( string_buffer );
  return [NSString stringWithCharacters:(unichar*)string_buffer.data length:string_buffer.count];
}

- (double) getReal64:(const char*)name
{
  int offset = [self locateKey:name];
  if (offset == -1) return 0.0;

  DataReader reader( buffer.data, buffer.count );
  reader.position = offset;

  switch (reader.read_int32x())
  {
    case  PLASMACORE_MESSAGE_DATA_TYPE_REAL64:
      return reader.read_real64();

    case  PLASMACORE_MESSAGE_DATA_TYPE_INT64:
      return reader.read_int64x();

    case  PLASMACORE_MESSAGE_DATA_TYPE_INT32:
    case  PLASMACORE_MESSAGE_DATA_TYPE_LOGICAL:
      return reader.read_int32x();

    default:
      return 0.0;
  }
}

- (Int64) getInt64:(const char*)name
{
  int offset = [self locateKey:name];
  if (offset == -1) return 0;

  DataReader reader( buffer.data, buffer.count );
  reader.position = offset;

  switch (reader.read_int32x())
  {
    case  PLASMACORE_MESSAGE_DATA_TYPE_REAL64:
      return (Int64) reader.read_real64();

    case  PLASMACORE_MESSAGE_DATA_TYPE_INT64:
      return reader.read_int64x();

    case  PLASMACORE_MESSAGE_DATA_TYPE_INT32:
    case  PLASMACORE_MESSAGE_DATA_TYPE_LOGICAL:
      return reader.read_int32x();

    default:
      return 0;
  }
}

- (int) getInt32:(const char*)name
{
  int offset = [self locateKey:name];
  if (offset == -1) return 0.0;

  DataReader reader( buffer.data, buffer.count );
  reader.position = offset;

  switch (reader.read_int32x())
  {
    case  PLASMACORE_MESSAGE_DATA_TYPE_REAL64:
      return (int) reader.read_real64();

    case  PLASMACORE_MESSAGE_DATA_TYPE_INT64:
      return (int) reader.read_int64x();

    case  PLASMACORE_MESSAGE_DATA_TYPE_INT32:
    case  PLASMACORE_MESSAGE_DATA_TYPE_LOGICAL:
      return reader.read_int32x();

    default:
      return 0.0;
  }
}

- (bool) getLogical:(const char*)name
{
  int offset = [self locateKey:name];
  if (offset == -1) return 0.0;

  DataReader reader( buffer.data, buffer.count );
  reader.position = offset;

  switch (reader.read_int32x())
  {
    case  PLASMACORE_MESSAGE_DATA_TYPE_REAL64:
      return reader.read_real64() != 0;

    case  PLASMACORE_MESSAGE_DATA_TYPE_INT64:
      return reader.read_int64x() != 0;

    case  PLASMACORE_MESSAGE_DATA_TYPE_INT32:
    case  PLASMACORE_MESSAGE_DATA_TYPE_LOGICAL:
      return reader.read_int32x() != 0;

    default:
      return false;
  }
}

- (NSData*)   getBytes:(const char*)name
{
  int offset = [self locateKey:name];
  if (offset == -1) return [NSData data];

  DataReader reader( buffer.data, buffer.count );
  reader.position = offset;

  if (reader.read_int32x() != PLASMACORE_MESSAGE_DATA_TYPE_BYTES) return [NSData data];

  int count = reader.read_int32x();
  Builder<Byte> byte_buffer;
  byte_buffer.reserve( count );
  for (int i=0; i<count; ++i) byte_buffer.add( reader.read_byte() );

  return [NSData dataWithBytes:byte_buffer.data length:byte_buffer.count];
}

@end

