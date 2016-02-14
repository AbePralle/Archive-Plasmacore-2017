//=============================================================================
//  SuperCPPStringBuilder.cpp
//
//  v1.1.1 - 2016.02.13 by Abe Pralle
//
//  See README.md for instructions.
//=============================================================================
#include "SuperCPPStringBuilder.h"

#include <cstdio>
#include <cstring>
using namespace std;

namespace PROJECT_NAMESPACE
{
namespace SuperCPP
{

//=============================================================================
//  StringBuilder
//=============================================================================
StringBuilder::StringBuilder( int initial_capacity )
{
  reserve( initial_capacity );
}

const char* StringBuilder::as_c_string()
{
  int c = count;
  reserve( (c+1)/2 + 1 );

  char* dest = (char*)(data + count);
  for (int i=0; i<c; ++i)
  {
    dest[i] = (char) data[i];
  }
  dest[c] = 0;
  return dest;
}

const char* StringBuilder::as_utf8_string()
{
  int c = encoded_utf8_count( data, count );
  reserve( (c+1)/2 + 1 );
  char* dest = (char*)(data + count);
  SuperCPP::encode_utf8( data, count, dest );
  dest[c] = 0;
  return dest;
}

StringBuilder& StringBuilder::clear()
{
  Builder<Character>::clear();
  return *this;
}

StringBuilder& StringBuilder::decode_utf8()
{
  // Decoding in place is fine since decoded UTF-8 will always be equal to
  // or smaller than the original.
  int decoded_count = decoded_utf8_count( data, count );
  SuperCPP::decode_utf8( data, count, data );
  count = decoded_count;
  return *this;
}

StringBuilder& StringBuilder::encode_utf8()
{
  int encoded_count = encoded_utf8_count( data, count );
  StringBuilder temp( encoded_count );
  SuperCPP::encode_utf8( data, count, temp.data );
  clear().reserve( encoded_count );
  memcpy( data, temp.data, encoded_count * sizeof(Character) );
  count = encoded_count;
  return *this;
}

StringBuilder& StringBuilder::print( bool value )
{
  return print( value ? "true" : "false" );
}

StringBuilder& StringBuilder::print( char value )
{
  add( value );
  return *this;
}

StringBuilder& StringBuilder::print( const char* value )
{
  int len = strlen( value );
  reserve( len );
  --value;
  for (char ch=*(++value); ch; ch=*(++value))
  {
    add( ch );
  }
  return *this;
}

StringBuilder& StringBuilder::print( Character value )
{
  add( value );
  return *this;
}

StringBuilder& StringBuilder::print( double value )
{
  char st[512];
  double_to_c_string( value, st, 512 );
  return print( st );
}

StringBuilder& StringBuilder::print( double value, int decimal_digits )
{
  char format[80];
  char st[80];
  snprintf( format, 80, "%%.%dlf", decimal_digits );
  snprintf( st, 80, format, value );
  return print( st );
}

StringBuilder& StringBuilder::print( int value )
{
  char st[40];
  sprintf( st, "%d", value );
  return print( st );
}

StringBuilder& StringBuilder::print( Int64 value )
{
  char st[40];
  sprintf( st, "%lld", value );
  return print( st );
}

StringBuilder& StringBuilder::print_character32( int value )
{
  if ((value & 0xffe00000) != 0)
  {
    return print( '?' );
  }
  else if (value <= 0xffff)
  {
    return print( (Character) value );
  }
  else
  {
    // Surrogate pair
    value -= 0x10000;
    print( (Character)(0xd800 + ((value >> 10) & 0x3ff)) );
    return print( (Character)(0xdc00 + (value & 0x3ff)) );
  }
}

StringBuilder& StringBuilder::println( bool value )
{
  return print( value ).print( '\n' );
}

StringBuilder& StringBuilder::println( char value )
{
  return print( value ).print( '\n' );
}

StringBuilder& StringBuilder::println( const char* value )
{
  return print( value ).print( '\n' );
}

StringBuilder& StringBuilder::println( Character value )
{
  return print( value ).print( '\n' );
}

StringBuilder& StringBuilder::println( double value )
{
  return print( value ).print( '\n' );
}

StringBuilder& StringBuilder::println( double value, int decimal_digits )
{
  return print( value, decimal_digits ).print( '\n' );
}

StringBuilder& StringBuilder::println( int value )
{
  return print( value ).print( '\n' );
}

StringBuilder& StringBuilder::println( Int64 value )
{
  return print( value ).print( '\n' );
}

StringBuilder& StringBuilder::println_character32( int value )
{
  return print_character32( value ).print( '\n' );
}

}; // namespace SuperCPP
}; // namespace PROJECT_NAMESPACE

