//=============================================================================
//  SuperCPPDataBuilder.h
//
//  v1.1.1 - 2016.02.13 by Abe Pralle
//
//  See README.md for instructions.
//=============================================================================
#ifndef SUPER_CPP_DATA_BUILDER_H
#define SUPER_CPP_DATA_BUILDER_H

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
//  DataBuilder
//=============================================================================
struct DataBuilder : Builder<Byte>
{
  DataBuilder( int initial_capacity=1024 );

  DataBuilder& write_byte( int value );
  DataBuilder& write_int64( Int64 value );
  DataBuilder& write_int64x( Int64 value );
  DataBuilder& write_int32( Int32 value );
  DataBuilder& write_int32x( Int32 value );
  DataBuilder& write_logical( bool value );
  DataBuilder& write_real64( Real64 value );
  DataBuilder& write_real32( Real32 value );
  DataBuilder& write_string( const char* characters, int character_count=-1 );
  DataBuilder& write_string( Character* characters, int character_count );
};

} // namespace SuperCPP
} // namespace PROJECT_NAMESPACE

using namespace PROJECT_NAMESPACE;

#endif // SUPER_CPP_DATA_BUILDER_H
