#include "PlasmacoreMessage.h"

#include "Plasmacore.h"

#include <string.h>

MID PlasmacoreMessage::next_message_id = 1;



PlasmacoreMessage::PlasmacoreMessage ( Buffer& data )
{
  init();
  this->data = data;
  type = readString();
  message_id = readIntX();
  while (indexAnother());
}

PlasmacoreMessage::PlasmacoreMessage (const char * type, MID message_id)
{
  init();
  this->type = type;
  this->message_id = message_id;
  writeString(type);
  writeIntX(message_id);
}

PlasmacoreMessage::PlasmacoreMessage (const char * type) : PlasmacoreMessage(type, PlasmacoreMessage::next_message_id++)
{
}

PlasmacoreMessage::PlasmacoreMessage (void)
{
  init();
}

void PlasmacoreMessage::init (void)
{
  type = "Unspecified";
  message_id = 0;
}

PlasmacoreMessage PlasmacoreMessage::createReply (void) const
{
  return PlasmacoreMessage("<reply>", message_id);
}

/*
Buffer PlasmacoreMessage::getBytes (const char * name)
{
  auto offset = entries.find(name);
  if (offset != entries.end())
  {
    position = offset->second;
    switch (readIntX())
    {
      case DataType::BYTES:
      {
        auto count = readIntX();
        Buffer buffer;
        buffer.reserve(count);
        for (int i = 0; i < count; ++i)
        {
          buffer.push_back(readByte());
        }
        return buffer;
      }
      default:
        break;
    }
  }
  return Buffer();
}
*/

int32_t PlasmacoreMessage::getInt32 (String & name, int32_t default_value)
{
  auto offset = entries.find(name);
  if (offset != entries.end())
  {
    position = offset->second;
    switch (readIntX())
    {
    case DataType::REAL64:
      return (int32_t)readReal64();

    case DataType::INT64:
      return (int32_t)readInt64X();

    case DataType::INT32:
      return readIntX();

    case DataType::LOGICAL:
      return readIntX();

    default:
      return default_value;
    }
  }
  return default_value;
}

int64_t PlasmacoreMessage::getInt64 (String & name, int64_t default_value)
{
  auto offset = entries.find(name);
  if (offset != entries.end())
  {
    position = offset->second;
    switch (readIntX())
    {
    case DataType::REAL64:
      return (int64_t)readReal64();

    case DataType::INT64:
      return readInt64X();

    case DataType::INT32:
      return (int64_t)readIntX();

    case DataType::LOGICAL:
      return readIntX();

    default:
      return default_value;
    }
  }
  return default_value;
}

bool PlasmacoreMessage::getLogical( String & name, bool default_value )
{
  return getInt32( name, default_value ? 1 : 0 ) != 0;
}

double PlasmacoreMessage::getReal64( String & name, double default_value )
{
  auto offset = entries.find(name);
  if (offset != entries.end())
  {
    position = offset->second;
    switch (readIntX())
    {
    case DataType::REAL64:
      return readReal64();

    case DataType::INT64:
      return double(readInt64X());

    case DataType::INT32:
      return double(readIntX());

    case DataType::LOGICAL:
      return double(readIntX());

    default:
      return default_value;
    }
  }
  return default_value;

}

String PlasmacoreMessage::getString( String & name, String & default_value )
{
  auto offset = entries.find(name);
  if (offset != entries.end())
  {
    position = offset->second;
    switch (readIntX())
    {
      case DataType::STRING:
        return readString();
      default:
        return default_value;
    }
  }
  return default_value;
}

String PlasmacoreMessage::getString( String & name )
{
  auto offset = entries.find(name);
  if (offset != entries.end())
  {
    position = offset->second;
    switch (readIntX())
    {
      case DataType::STRING:
        return readString();
      default:
        return "";
    }
  }
  return "";
}

bool PlasmacoreMessage::indexAnother (void)
{
  if (position == data.count) { return false; }
  auto name = readString();
  entries[ name ] = position;

  switch (readIntX())
  {
  case DataType::STRING:
  {
    auto count = readIntX();
    for (int i = 0; i < count; i++) readIntX();
    return true;
  }
  case DataType::REAL64:
  {
    readReal64();
    return true;
  }
  case DataType::INT64:
  {
    readInt64X();
    return true;
  }
  case DataType::INT32:
  {
    readIntX();
    return true;
  }
  case DataType::LOGICAL:
  {
    readIntX();
    return true;
  }
  case DataType::BYTES:
  {
    position += readIntX();
    return true;
  }
  default:
  {
    return false;
  }
  }
}

void PlasmacoreMessage::send()
{
  Plasmacore::singleton.send( *this );
}

void PlasmacoreMessage::send_rsvp( HandlerCallback callback )
{
  Plasmacore::singleton.send_rsvp( *this, callback );
}

PlasmacoreMessage & PlasmacoreMessage::set( const char * name, Buffer & value )
{
  position = data.count;
  writeString( name );
  entries[ name ] = position;
  writeIntX( DataType::BYTES );
  writeIntX( value.count );
  for (int i = 0; i < value.count; ++i)
  {
    writeIntX( value[i] );
  }
  return *this;
}

PlasmacoreMessage & PlasmacoreMessage::set( const char * name, int64_t value )
{
  position = data.count;
  writeString( name );
  entries[ name ] = position;
  writeIntX( DataType::INT64 );
  writeIntX( Int(value>>32) );
  writeIntX( Int(value) );
  return *this;
}

PlasmacoreMessage & PlasmacoreMessage::set( const char * name, Int value )
{
  position = data.count;
  writeString( name );
  entries[ name ] = position;
  writeIntX( DataType::INT32 );
  writeIntX( value );
  return *this;
}

PlasmacoreMessage & PlasmacoreMessage::set( const char * name, bool value )
{
  position = data.count;
  writeString( name );
  entries[ name ] = position;
  writeIntX( DataType::LOGICAL );
  writeIntX( value ? 1 : 0 );
  return *this;
}

PlasmacoreMessage & PlasmacoreMessage::set( const char * name, double value )
{
  position = data.count;
  writeString( name );
  entries[ name ] = position;
  writeIntX( DataType::REAL64 );
  writeReal64( value );
  return *this;
}

PlasmacoreMessage & PlasmacoreMessage::set( const char * name, const char * value )
{
  position = data.count;
  writeString( name );
  entries[ name ] = position;
  writeIntX( DataType::STRING );
  writeString( value );
  return *this;
}

PlasmacoreMessage & PlasmacoreMessage::set( const char * name, String & value )
{
  position = data.count;
  writeString( name );
  entries[ name ] = position;
  writeIntX( DataType::STRING );
  writeString( value );
  return *this;
}


//---------------------------------------------------------------------------
// PRIVATE
//---------------------------------------------------------------------------

Int PlasmacoreMessage::readByte (void)
{
  if (position >= data.count) { return 0; }
  position += 1;
  return Int(data[position-1]);
}

int64_t PlasmacoreMessage::readInt64X (void)
{
  auto result = int64_t( readIntX() ) << 32;
  return result | int64_t( uint32_t(readIntX()) );
}

int32_t PlasmacoreMessage::readInt32()
{
  auto result = readByte();
  result = (result << 8) | readByte();
  result = (result << 8) | readByte();
  return (result << 8) | readByte();
}

Int PlasmacoreMessage::readIntX()
{
  auto b = readByte();
  if ((b & 0xc0) != 0x80)
  {
    return b;
  }

  switch (b & 0x30)
  {
    case 0x00:
      return ((b & 15) << 8) | readByte();

    case 0x10:
      position += 2;
      if (position > data.count)
      {
        position = data.count;
        return 0;
      }
      return ((b & 15) << 16) | (Int(data[position-2])<<8) | Int(data[position-1]);

    case 0x20:
      position += 3;
      if (position > data.count)
      {
        position = data.count;
        return 0;
      }
      return ((b & 15) << 24) | (Int(data[position-3])<<16) | (Int(data[position-2]<<8)) | Int(data[position-1]);

    default:
      return readInt32();
  }
}

double PlasmacoreMessage::readReal64 (void)
{
  auto n = uint64_t( readInt32() ) << 32;
  n = n | uint64_t( uint32_t(readInt32()) );
  return *((double*)&n);
}

String PlasmacoreMessage::readString (void)
{
  std::string string_buffer;
  auto count = readIntX();
  for (int i = 0; i < count; ++i)
  {
    string_buffer.push_back( char(readIntX()) );
  }
  return String( string_buffer );
}

void PlasmacoreMessage::writeByte ( Int value )
{
  position += 1;
  if (position > data.count)
  {
    data.add( uint8_t(value&255) );
  }
  else
  {
    data[ position-1 ] = uint8_t( value );
  }
}

void PlasmacoreMessage::writeInt32( Int value )
{
  writeByte( value >> 24 );
  writeByte( value >> 16 );
  writeByte( value >> 8  );
  writeByte( value );
}

void PlasmacoreMessage::writeInt64X( int64_t value )
{
  writeIntX( Int(value>>32) );
  writeIntX( Int(value) );
}

void PlasmacoreMessage::writeIntX( Int value )
{
  if (value >= -64 && value <= 127)
  {
    writeByte( value );
  }
  else if (value >= -0x400 && value <= 0x3ff)
  {
    writeByte( 0x80 | ((value >> 8) & 15) );
    writeByte( value );
  }
  else if (value >= -0x40000 && value <= 0x3ffff)
  {
    writeByte( 0x90 | ((value >> 16) & 15) );
    writeByte( value >> 8 );
    writeByte( value );
  }
  else if (value >= -0x4000000 && value <= 0x3ffffff)
  {
    writeByte( 0xa0 | ((value >> 24) & 15) );
    writeByte( value >> 16 );
    writeByte( value >> 8 );
    writeByte( value );
  }
  else
  {
    writeByte( 0xb0 );
    writeInt32( value );
  }
}

void PlasmacoreMessage::writeReal64 ( double value )
{
  uint64_t bits = *(uint64_t*)(&value);
  writeInt32( Int((bits>>32)&0xFFFFffff) );
  writeInt32( Int(bits&0xFFFFffff) );
}

void PlasmacoreMessage::writeString ( const char * value )
{
  int len = strlen(value);
  writeIntX( len );
  for (int i = 0; i < len; ++i)
  {
    writeIntX( Int(value[i]) );
  }
}

void PlasmacoreMessage::writeString ( String & value )
{
  int len = value.size();
  writeIntX( len );
  for (int i = 0; i < len; ++i)
  {
    writeIntX( Int(value[i]) );
  }
}
