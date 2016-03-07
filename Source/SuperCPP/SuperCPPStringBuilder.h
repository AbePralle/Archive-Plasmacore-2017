//=============================================================================
//  SuperCPPStringBuilder.h
//
//  v1.1.3 - 2016.03.05 by Abe Pralle
//
//  See README.md for instructions.
//=============================================================================
#ifndef SUPER_CPP_STRING_BUILDER_H
#define SUPER_CPP_STRING_BUILDER_H

#ifndef PROJECT_NAMESPACE
  #define PROJECT_NAMESPACE Project
#endif

#include "SuperCPP.h"
#include "SuperCPPBuilder.h"

namespace PROJECT_NAMESPACE
{
namespace SuperCPP
{

//=============================================================================
//  StringBuilder
//=============================================================================
struct StringBuilder : Builder<Character>
{
  StringBuilder( int initial_capacity=1024 );

  const char* as_c_string();
  const char* as_utf8_string();

  StringBuilder& clear();

  StringBuilder& decode_utf8();
  StringBuilder& encode_utf8();

  StringBuilder& print( bool value );
  StringBuilder& print( char value );
  StringBuilder& print( const char* value );
  StringBuilder& print( Character value );
  StringBuilder& print( double value );
  StringBuilder& print( double value, int decimal_digits );
  StringBuilder& print( int value );
  StringBuilder& print( Int64 value );
  StringBuilder& print_character32( int value );

  StringBuilder& println( bool value );
  StringBuilder& println( char value );
  StringBuilder& println( const char* value );
  StringBuilder& println( Character value );
  StringBuilder& println( double value );
  StringBuilder& println( double value, int decimal_digits );
  StringBuilder& println( int value );
  StringBuilder& println( Int64 value );
  StringBuilder& println_character32( int value );
};

} // namespace SuperCPP
} // namespace PROJECT_NAMESPACE

using namespace PROJECT_NAMESPACE;

#endif // SUPER_CPP_STRING_BUILDER_H
