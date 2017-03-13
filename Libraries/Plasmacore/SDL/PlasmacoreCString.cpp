//=============================================================================
//  PlasmacoreCString.cpp
//=============================================================================
#include "PlasmacoreCString.h"

#include <cstring>
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

