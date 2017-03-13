//=============================================================================
//  PlasmacoreUtility.cpp
//=============================================================================
#include "PlasmacoreUtility.h"

#include <cstring>
#include <cstdio>
using namespace std;

using namespace std;

//=============================================================================
//  PlasmacoreCString
//=============================================================================
PlasmacoreCString::PlasmacoreCString() : characters(0), count(0)
{
}

PlasmacoreCString::PlasmacoreCString( const char* value ) : characters(0), count(0)
{
  *this = value;
}

PlasmacoreCString::PlasmacoreCString( const PlasmacoreCString& existing ) : characters(0), count(0)
{
  *this = existing;
}

PlasmacoreCString::~PlasmacoreCString()
{
  if (characters) delete characters;
}

PlasmacoreCString& PlasmacoreCString::operator=( const char* value )
{
  if (characters) delete characters;

  if ( !value )
  {
    characters = 0;
    count = 0;
    return *this;
  }

  count = (int) strlen( value );
  characters = new char[ count+1 ];
  strcpy( characters, value );
  return *this;
}

PlasmacoreCString& PlasmacoreCString::operator=( const PlasmacoreCString& other )
{
  if (characters) delete characters;

  count = other.count;
  if ( !count )
  {
    characters = 0;
    return *this;
  }

  this->characters = new char[ count+1 ];
  strcpy( this->characters, other.characters );
  return *this;
}

PlasmacoreCString::operator char*()
{
  if (characters) return characters;
  return (char*) "";
}

PlasmacoreCString::operator const char*()
{
  if (characters) return characters;
  return "";
}

bool PlasmacoreCString::operator==( const PlasmacoreCString& other )
{
  if ( !characters )        return !other.characters;
  if (count != other.count) return false;
  return (0 == strcmp(characters,other.characters));
}

bool PlasmacoreCString::operator==( const char* st )
{
  if ( !characters ) return !st;
  return (0 == strcmp(characters,st));
}


//=============================================================================
//  PlasmacoreCStringBuilder
//=============================================================================
PlasmacoreCStringBuilder::PlasmacoreCStringBuilder()
{
}

PlasmacoreCStringBuilder::~PlasmacoreCStringBuilder()
{
  if (data != internal_buffer) delete data;
  capacity = count = 0;
}

PlasmacoreCStringBuilder& PlasmacoreCStringBuilder::add( char value )
{
  reserve( 1 );
  data[ count++ ] = value;
  return *this;
}

char* PlasmacoreCStringBuilder::as_c_string()
{
  reserve( 1 )[ count ] = 0;  // temporary null terminator
  return data;
}

PlasmacoreCStringBuilder& PlasmacoreCStringBuilder::clear()
{
  count = 0;
  return *this;
}

char& PlasmacoreCStringBuilder::operator[]( int index )
{
  return data[ index ];
}

PlasmacoreCStringBuilder& PlasmacoreCStringBuilder::reserve( int additional_capacity )
{
  int required_capacity = count + additional_capacity;
  if (capacity >= required_capacity) return *this;

  int double_capacity = count << 1;
  if (double_capacity > required_capacity) required_capacity = double_capacity;

  char* new_data = new char[ required_capacity ];
  memcpy( new_data, data, count );

  if (data != internal_buffer) delete data;
  data = new_data;

  capacity = required_capacity;
  return *this;
}

