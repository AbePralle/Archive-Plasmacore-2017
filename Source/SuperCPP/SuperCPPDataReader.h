//=============================================================================
//  SuperCPPDataReader.h
//
//  v1.1.3 - 2016.03.05 by Abe Pralle
//
//  See README.md for instructions.
//=============================================================================
#ifndef SUPER_CPP_DATA_READER_H
#define SUPER_CPP_DATA_READER_H

#ifndef PROJECT_NAMESPACE
  #define PROJECT_NAMESPACE Project
#endif

#include "SuperCPP.h"
#include "SuperCPPStringBuilder.h"

namespace PROJECT_NAMESPACE
{
namespace SuperCPP
{

//=============================================================================
//  DataReader
//=============================================================================
struct DataReader
{
  // PROPERTIES
  bool  should_free_data;
  int   position;
  int   count;
  Byte* data;

  // METHODS
  DataReader();
  DataReader( Byte* data, int count, bool should_free=false );
  ~DataReader();

  bool   has_another();
  int    read_byte();
  Int64  read_int64();
  Int64  read_int64x();
  Int32  read_int32();
  Int32  read_int32x();
  bool   read_logical();
  Real64 read_real64();
  Real32 read_real32();
  int    read_string( StringBuilder& buffer );
  void   reset( Byte* data, int count, bool should_free=false );
  int    remaining();
};

} // namespace SuperCPP
} // namespace PROJECT_NAMESPACE

using namespace PROJECT_NAMESPACE;

#endif // SUPER_CPP_DATA_READER_H
