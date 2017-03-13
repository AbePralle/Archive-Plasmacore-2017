//=============================================================================
//  PlasmacoreCStringBuilder.h
//=============================================================================
#ifndef PLASMACORE_C_STRING_BUILDER_H
#define PLASMACORE_C_STRING_BUILDER_H

#include <cstring>

//=============================================================================
//  PlasmacoreCStringBuilder
//=============================================================================
struct PlasmacoreCStringBuilder
{
  // PROPERTIES
  char* data;
  int   count;
  int   capacity;
  char  internal_buffer[ 1024 ];

  // METHODS
  PlasmacoreCStringBuilder() : data(internal_buffer), count(0), capacity(1024)
  {
  }

  ~PlasmacoreCStringBuilder()
  {
    if (data != internal_buffer) delete data;
    capacity = count = 0;
  }

  PlasmacoreCStringBuilder& add( char value )
  {
    reserve( 1 );
    data[ count++ ] = value;
    return *this;
  }

  PlasmacoreCStringBuilder& clear()
  {
    count = 0;
    return *this;
  }

  char& operator[]( int index )
  {
    return data[ index ];
  }

  PlasmacoreCStringBuilder& reserve( int additional_capacity )
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
};

#endif // PLASMACORE_C_STRING_BUILDER_H
