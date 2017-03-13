//=============================================================================
//  PlasmacoreCString.h
//=============================================================================
#ifndef PLASMACORE_C_STRING_H
#define PLASMACORE_C_STRING_H

//=============================================================================
//  PlasmacoreCString
//=============================================================================
struct PlasmacoreCString
{
  char* characters;
  int   count;

  PlasmacoreCString();
  PlasmacoreCString( const char* value );
  PlasmacoreCString( const PlasmacoreCString& existing );
  ~PlasmacoreCString();

  PlasmacoreCString& operator=( const char* value );
  PlasmacoreCString& operator=( const PlasmacoreCString& other );
  inline char operator[]( int index ) { return characters[index]; }
  operator char*();
  operator const char*();
  bool operator==( const PlasmacoreCString& other );
  bool operator==( const char* st );
};

#endif // PLASMACORE_C_STRING_H
