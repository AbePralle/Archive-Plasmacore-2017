//=============================================================================
//  SuperCPPDataReader.cpp
//
//  v1.1.3 - 2016.03.05 by Abe Pralle
//
//  See README.md for instructions.
//=============================================================================
#include "SuperCPPDataReader.h"

#include <cstdio>
using namespace std;

namespace PROJECT_NAMESPACE
{
namespace SuperCPP
{

//=============================================================================
//  DataReader
//=============================================================================
DataReader::DataReader() : should_free_data(false), position(0), count(0), data(0)
{
}

DataReader::DataReader( Byte* data, int count, bool should_free )
  : should_free_data(should_free), position(0), count(count), data(data)
{
}

DataReader::~DataReader()
{
  if (should_free_data && data) delete data;
}

bool DataReader::has_another()
{
  return (position < count);
}

int DataReader::read_byte()
{
  if (position == count) return 0;
  return data[ position++ ];
}

Int64 DataReader::read_int64()
{
  Int64 result = read_int32();
  return (result << 32) | (read_int32() & 0xffffffffLL);
}

Int64 DataReader::read_int64x()
{
  Int64 result = read_int32x();
  return (result << 32) | (read_int32x() & 0xffffffffLL);
}

Int32 DataReader::read_int32()
{
  if ((position+=4) > count)
  {
    position = count;
    return 0;
  }

  return (Int32)((data[position-4]<<24) | (data[position-3]<<16) 
       | (data[position-2]<<8) | data[position-1]);
}

Int32 DataReader::read_int32x()
{
  Int32 b = (char) read_byte();
  if ((b & 0xc0) != 0x80) return b;

  switch (b & 0x30)
  {
    case 0x00:
      b = ((char)(b << 4)) << 4;
      return b | read_byte();

    case 0x10:
      if ((position+=2) > count)
      {
        position = count;
        return 0;
      }
      b = ((char)(b << 4)) << 12;
      return b | (data[position-2]<<8) | data[position-1];

    case 0x20:
      if ((position+=3) > count)
      {
        position = count;
        return 0;
      }
      b = ((char)(b << 4)) << 20;
      return b | (data[position-3]<<16) | (data[position-2]<<8) | data[position-1];

    default:
      return read_int32();
  }
}

bool DataReader::read_logical()
{
  return !!read_byte();
}

Real64 DataReader::read_real64()
{
  Int64 value = read_int64();
  return *((Real64*)&value);
}

Real32 DataReader::read_real32()
{
  Int32 value = read_int32();
  return *((Real32*)&value);
}

int DataReader::read_string( StringBuilder& buffer )
{
  int character_count = read_int32x();
  if (character_count > count - position)
  {
    // Prevent protocol logic bugs from causing us to loop millions of times
    character_count = count - position;
  }

  buffer.reserve( character_count );

  for (int i=0; i<character_count; ++i)
  {
    buffer.add( (Character) read_int32x() );
  }

  return character_count;
}

int DataReader::remaining()
{
  return (count - position);
}

void DataReader::reset( Byte* data, int count, bool should_free )
{
  if (should_free_data && data) delete data;
  should_free_data = should_free;
  position = 0;
  this->count = count;
  this->data = data;
}

}; // namespace SuperCPP
}; // namespace PROJECT_NAMESPACE

