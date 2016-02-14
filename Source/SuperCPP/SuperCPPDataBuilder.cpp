//=============================================================================
//  SuperCPPDataBuilder.cpp
//
//  v1.1.1 - 2016.02.13 by Abe Pralle
//
//  See README.md for instructions.
//=============================================================================

#include "SuperCPPDataBuilder.h"

namespace PROJECT_NAMESPACE
{
namespace SuperCPP
{

//=============================================================================
//  DataBuilder
//=============================================================================
DataBuilder::DataBuilder( int initial_capacity )
{
  reserve( initial_capacity );
}

DataBuilder& write_byte( int value );
DataBuilder& write_int64( Int64 value );

DataBuilder& DataBuilder::write_byte( int value )
{
  add( value );
  return *this;
}

DataBuilder& DataBuilder::write_int64( Int64 value )
{
  write_int32( (Int32) (value >> 32) );
  return write_int32( (Int32) value );
}

DataBuilder& DataBuilder::write_int64x( Int64 value )
{
  write_int32x( value >> 32 );
  return write_int32x( value );
}

DataBuilder& DataBuilder::write_logical( bool value )
{
  return write_byte( value ? 1 : 0 );
}

DataBuilder& DataBuilder::write_real64( Real64 value )
{
  write_int64( *((Int64*)(&value)) );
  return *this;
}

DataBuilder& DataBuilder::write_real32( Real32 value )
{
  write_int32( *((Int32*)(&value)) );
  return *this;
}

DataBuilder& DataBuilder::write_int32( Int32 value )
{
  add( value >> 24 ).add( value >> 16 ).add( value >> 8 ).add( value );
  return *this;
}

DataBuilder& DataBuilder::write_int32x( Int32 value )
{
  if (value >= -64 && value <= 127)
  {
    add( value );
  }
  else if (value >= -0x400 && value <= 0x3ff)
  {
    add( 0x80 | ((value >> 8) & 15) );
    add( value );
  }
  else if (value >= -0x40000 && value <= 0x3ffff)
  {
    add( 0x90 | ((value >> 16) & 15) );
    add( value >> 8 );
    add( value );
  }
  else if (value >= -0x4000000 && value <= 0x3ffffff)
  {
    add( 0xa0 | ((value >> 24) & 15) );
    add( value >> 16 );
    add( value >> 8 );
    add( value );
  }
  else
  {
    add( 0xb0 );
    add( value >> 24 );
    add( value >> 16 );
    add( value >> 8 );
    add( value );
  }
  return *this;
}

DataBuilder& DataBuilder::write_string( const char* characters, int character_count )
{
  if (character_count == -1) character_count = strlen( characters );

  write_int32x( character_count );
  for (int i=0; i<character_count; ++i)
  {
    write_int32x( characters[i] );
  }
  return *this;
}

DataBuilder& DataBuilder::write_string( Character* characters, int character_count )
{
  write_int32x( character_count );
  for (int i=0; i<character_count; ++i)
  {
    write_int32x( characters[i] );
  }
  return *this;
}

}; // namespace SuperCPP
}; // namespace PROJECT_NAMESPACE

