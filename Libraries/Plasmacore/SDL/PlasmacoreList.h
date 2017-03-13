//=============================================================================
//  PlasmacoreList.h
//
//  v1.1.3 - 2016.03.05 by Abe Pralle
//
//  See README.md for instructions.
//=============================================================================
#ifndef PLASMACORE_LIST_H
#define PLASMACORE_LIST_H

#include <cstring>

//=============================================================================
//  PlasmacoreList
//=============================================================================
template <class DataType>
struct PlasmacoreList
{
  // PROPERTIES
  DataType* data;
  int       count;
  int       capacity;

  // METHODS
  PlasmacoreList( int initial_capacity=0 ) : data(0), count(0), capacity(0)
  {
    reserve( initial_capacity );
  }

  ~PlasmacoreList()
  {
    if (data) delete data;
    capacity = count = 0;
  }

  PlasmacoreList<DataType>& add( DataType value )
  {
    reserve( 1 );
    data[ count++ ] = value;
    return *this;
  }

  PlasmacoreList<DataType>& add( DataType* array, int array_count )
  {
    reserve( array_count );
    std::memcpy( data + count, array, array_count*sizeof(DataType) );
    count += array_count;
    return *this;
  }

  PlasmacoreList<DataType>& add( PlasmacoreList<DataType>& other )
  {
    int n = other.count;
    reserve( n );
    if (n)
    {
      std::memcpy( data, other.data, n*sizeof(DataType) );
    }
    count += n;
    return *this;
  }

  PlasmacoreList<DataType>& adopt( DataType* new_data, int new_count )
  {
    if (data) delete data;
    data = new_data;
    count = new_count;
    capacity = new_count;
  }

  PlasmacoreList<DataType>& clear()
  {
    count = 0;
    return *this;
  }

  DataType first() { return data[0]; }

  PlasmacoreList<DataType>& insert( DataType value, int before_index )
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

  DataType& operator=( const DataType& other )
  {
    return clear().add( other );
  }

  DataType remove_at( int index )
  {
    if (index == count - 1)
    {
      return remove_last();
    }
    else
    {
      DataType result = data[ index ];
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

  PlasmacoreList<DataType>& reserve( int additional_capacity )
  {
    int required_capacity = count + additional_capacity;
    if (capacity >= required_capacity) return *this;

    if (data)
    {
      int double_capacity = count << 1;
      if (double_capacity > required_capacity) required_capacity = double_capacity;

      DataType* new_data = new DataType[ required_capacity ];
      std::memcpy( new_data, data, count * sizeof(DataType) );
      delete data;
      data = new_data;
    }
    else
    {
      if (required_capacity < 10) required_capacity = 10;
      data = new DataType[ required_capacity ];
    }

    capacity = required_capacity;
    return *this;
  }

  PlasmacoreList<DataType>& set( int index, DataType value )
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

#endif // PLASMACORE_LIST_H
