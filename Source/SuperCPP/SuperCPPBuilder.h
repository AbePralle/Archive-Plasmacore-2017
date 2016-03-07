//=============================================================================
//  SuperCPPBuilder.h
//
//  v1.1.3 - 2016.03.05 by Abe Pralle
//
//  See README.md for instructions.
//=============================================================================
#ifndef SUPER_CPP_BUILDER_H
#define SUPER_CPP_BUILDER_H

#ifndef PROJECT_NAMESPACE
  #define PROJECT_NAMESPACE Project
#endif

#include <cstring>

namespace PROJECT_NAMESPACE
{
namespace SuperCPP
{

//=============================================================================
//  Builder
//=============================================================================
template <class DataType>
struct Builder
{
  // PROPERTIES
  DataType* data;
  int       count;
  int       capacity;
  DataType  internal_buffer[ 1024 / sizeof(DataType) ];

  // METHODS
  Builder() : data(internal_buffer), count(0), capacity(1024/sizeof(DataType))
  {
  }

  ~Builder()
  {
    if (data != internal_buffer) delete data;
    capacity = count = 0;
  }

  void reset()
  {
    if (data != internal_buffer) delete data;
    data = internal_buffer;
    count = 0;
    capacity = 1024 / sizeof(DataType);
  }

  Builder<DataType>& add( DataType value )
  {
    reserve( 1 );
    data[ count++ ] = value;
    return *this;
  }

  Builder<DataType>& add( DataType* array, int array_count )
  {
    reserve( array_count );
    std::memcpy( data + count, array, array_count*sizeof(DataType) );
    count += array_count;
    return *this;
  }

  Builder<DataType>& adopt( DataType* new_data, int new_count )
  {
    if (data != internal_buffer) delete data;
    data = new_data;
    count = new_count;
    capacity = new_count;
  }

  Builder<DataType>& clear()
  {
    count = 0;
    return *this;
  }

  DataType first() { return data[0]; }

  Builder<DataType>& insert( DataType value, int before_index )
  {
    if (before_index >= count)
    {
      add( value );
      return *this;
    }

    if (before_index < 0) before_index = 0;

    reserve( 1 );

    memmove( 
        data + before_index + 1,
        data + before_index,
        (count - before_index) * sizeof(DataType)
      );
    data[ before_index ] = value;
    ++count;

    return *this;
  }

  DataType last() { return data[count-1]; }

  int locate( DataType value )
  {
    int  count = this->count;
    int* data = this->data;

    for (int i=0; i<count; ++i)
    {
      if (data[i] == value) return i;
    }

    return -1;
  }

  DataType& operator[]( int index )
  {
    return data[ index ];
  }

  DataType remove_at( int index )
  {
    if (index == count - 1)
    {
      return remove_last();
    }
    else
    {
      int result = data[ index ];
      memmove( data + index, data + index + 1, (--count - index) * sizeof(DataType) );
      return result;
    }
  }

  DataType remove_first()
  {
    return remove_at( 0 );
  }

  DataType remove_last()
  {
    return data[ --count ];
  }

  Builder<DataType>& reserve( int additional_capacity )
  {
    int required_capacity = count + additional_capacity;
    if (capacity >= required_capacity) return *this;

    int double_capacity = count << 1;
    if (double_capacity > required_capacity) required_capacity = double_capacity;

    DataType* new_data = new DataType[ required_capacity ];
    std::memcpy( new_data, data, count * sizeof(DataType) );

    if (data != internal_buffer) delete data;
    data = new_data;

    capacity = required_capacity;
    return *this;
  }

  Builder<DataType>& set( int index, DataType value )
  {
    reserve( (index+1) - count );
    if (count <= index) count = index + 1;

    data[ index ] = value;
    return *this;
  }

  DataType* to_array( DataType* array=0, int array_count=-1 )
  {
    int copy_count = count;
    if (array)
    {
      if (array_count >= 0 && array_count < copy_count) copy_count = array_count;
    }
    else
    {
      array = new DataType[ count ];
    }
    std::memcpy( array, data, copy_count * sizeof(DataType) );
    return array;
  }
};

} // namespace SuperCPP
} // namespace PROJECT_NAMESPACE

using namespace PROJECT_NAMESPACE;

#endif // SUPER_CPP_BUILDER_H
